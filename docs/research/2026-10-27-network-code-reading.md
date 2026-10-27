# Research: the original's network code (0.7.0 slice 1, #45)

Status: written 2026-10-27 for slice 1 of `docs/specs/2026-10-27-online-play-0.7.0-spec.md`. No game code changes. No build ran, because the 0.6.0 gates hold this Mac. Section 10 holds the results of slice 2 (real sockets).

Method: I disassembled the original 3.9.68 (`clean-3968/cm0102.exe`) with capstone and pefile, and resolved each import thunk to its IAT entry. Ghidra stayed closed for the gates. Every address below is one I read. Function map names are given as hints only. Where a name in this note differs from the function map, this note wins.

Address rule: memory = file offset + 0x400000 in `.text`.

## 0. Two facts that change the brief

1. The game imports `ioctlsocket` (WSOCK32 ordinal 12) and `gethostbyaddr` (ordinal 51). It does not import `inet_ntoa` (ordinal 11) or `gethostbyname` (ordinal 52). The IAT is 0x967214..0x967258, the thunks are 0x944c28..0x944c8e. `src/recomp_rt/win/net.c` labels ordinal 12 as `inet_ntoa` and ordinal 51 as `gethostbyname`. Both labels are wrong. Today this does no harm, because `socket` always fails first.
2. The address lookup 0x8aee20 does not call `CreateProcessA` itself. It calls the C runtime's `system` (0x9473ea, which reads `COMSPEC`) on Windows 2000 and later, and a spawn helper (0x9533ca) on Windows 9x and NT 4.

The 18 imports are: `WSAStartup`, `WSACleanup`, `WSAGetLastError`, `gethostname`, `gethostbyaddr`, `socket`, `accept`, `bind`, `connect`, `listen`, `recv`, `recvfrom`, `send`, `sendto`, `setsockopt`, `ioctlsocket`, `closesocket`, `htons`. There is no `select`, `WSAAsyncSelect`, `inet_addr`, `inet_ntoa` or `gethostbyname`.

## 1. Objects and where they live

| Object | Where | What I read |
|---|---|---|
| Screen manager | global 0xb74450 | Its constructor 0x7fc380 builds a 50,000-byte network buffer at +0x37fe (0x7fc39c) and two Displays (+0x3844, +0x132ea5). The network session pointer is +0x3840, so the global 0xb77c90. |
| Send buffer | global 0xae28f8 | 50,000 bytes (0x55eef0). Display serialisers and click senders write here. |
| Display packet buffer | global 0xae2908 | 3,072 bytes (0x55ef20). |
| `Network` (network.CPP) | heap, 0xc3e54 bytes (0x7ff562) | A `TcpIp` object with these fields after it: player slot sockets +0x4ba (16 shorts, slot 1 at +0x4ba), receive state +0x51a and length +0x4da (one dword per slot), one 50,000-byte receive buffer per slot at +0x55a (16 x 50,000 = 0xc3500, 0x77b59c), connected flag +0xc3a5a, host flag +0xc3a5e, connect and disconnect callbacks +0xc3a62 and +0xc3a66, game list count +0xc3a6a, game names +0xc3a6c (50 bytes each), host names or addresses +0xc3c60 (50 bytes each). |
| `TcpIp` (tcpip.cpp) | base of `Network` | +0 connection socket table, +4 table size (16, from 0x77a7eb), +6 adapter count, +8 adapter table (20 entries of 32 bytes), +0x288 twenty UDP sockets, +0x2d8 twenty TCP listen sockets, +0x328 WSADATA, +0x4b6 "Winsock started" flag. |

## 2. The UDP game list broadcast

Port: 6667 (0x1a0b) for UDP and TCP. The constant appears at 0x77a848, 0x77aa2f, 0x77aab8, 0x77a923, 0x77b07b, 0x81f921, 0x81fdf7 and in the loopback test 0x8af3e5 and 0x8af4bc.

### Sockets

`Network::Network` 0x77a7d0 starts Winsock (0x8ae260) and opens the UDP sockets with `TcpIp::open_broadcast_sockets` 0x8aea80 on port 6667. It ignores the result (0x77a84f..0x77a858). Only a failed Winsock start sets "not OK".

`open_broadcast_sockets` 0x8aea80 opens one UDP socket for each adapter that 0x8aee20 returns (up to 20):

1. `socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)` 0x8aeb1a.
2. `setsockopt(SOL_SOCKET, SO_BROADCAST, 1)` 0x8aeb41. On failure it closes the socket.
3. `ioctlsocket(FIONBIO, 1)` 0x8aeb72: non-blocking. The result is not checked.
4. `setsockopt SO_LINGER {1, 0}` 0x8aeb9e and `SO_DEBUG 1` 0x8aebb9. Not checked.
5. `bind` to the adapter's own address, port 6667 (0x8aec05). It sets no `SO_REUSEADDR` on UDP.
6. On a bind failure it calls `closesocket` twice on the same handle (0x8aec17, 0x8aec24).

### Packets

All three are plain text tags with no version field.

| Packet | Sent by | Layout | Length |
|---|---|---|---|
| `CM3 LIST SERVER REQ` | The game list, on build (0x81f906..0x81f930), on Refresh, button 0x15 (0x81fc2d..0x81fc4d), and after a failed join (0x81fddc..0x81fe06) | The 19 characters and the terminating zero | 20 bytes (strlen + 1) |
| `CM3 LIST SERVER ACK` | The host, once when it starts (0x77aa7c..0x77aafd), and as a reply to each REQ (0x77aff2..0x77b088) | Bytes 0..18: tag, no zero. Bytes 19..68: the host's `gethostname` text (50 bytes from +0xc3c60). Bytes 69..118: the game name (50 bytes from +0xc3a6c). | 119 bytes (strlen + 1 + 0x63) |
| `CM3 LIST SERVER BYE` | The host, in `Network::~Network` 0x77a890, only when the host flag is set | Bytes 0..18: tag. Bytes 19..68: the host name. Bytes 69..118 are stack bytes that the code does not set. | 119 bytes |

Where each goes:

- The start-up ACK and the BYE go to the broadcast address of every adapter (`TcpIp::send_datagram` 0x8aec90 with no target, `sendto` at 0x8aed1a, target = adapter entry +0x10, port 6667).
- The REQ also goes to every adapter's broadcast address.
- The reply ACK goes to the sender of the REQ, through the socket that received it (`send_datagram` with a target, `sendto` at 0x8aed5a).

The broadcast address is the adapter's subnet broadcast: address AND mask, OR NOT mask (0x8af24c..0x8af267). It is never 255.255.255.255.

### How the host answers

`Network::receive_packet` 0x77af20, host branch:

1. Read one datagram from the first UDP socket that has one (`receive_datagram` 0x8aed80, `recvfrom` 0x8aedcd, 2,000-byte buffer at 0x77af9f).
2. If the bytes equal `CM3 LIST SERVER REQ` with `strcmp` (0x77afb4..0x77afec), so the zero must be in the datagram, send the ACK back to the sender (0x77b061..0x77b088).
3. The `from` length passed to `recvfrom` is a stack slot that the function never sets (0x77af90, read again at 0x77b016). The real WinSock checks it. A runtime must ignore its value.

### How the game list fills

`Network::receive_packet`, joiner branch (0x77b092..0x77b45e):

1. An ACK is any datagram whose first 19 bytes match the tag (`strncmp`-like call 0x946fa0 at 0x77b0d4).
2. The list takes at most 9 games: an ACK is ignored when the count is 9 or more (0x77b0eb).
3. The address of the entry is the datagram's sender address, written as dotted text (`format_ipv4_address` 0x8ae140 at 0x77b10d). The host name inside the ACK is not used.
4. The game name comes from bytes 69..118 of the ACK (0x77b12b..0x77b143).
5. A second ACK from an address already in the list is dropped (0x77b145..0x77b1b1).
6. The entry is kept only if `gethostbyaddr` finds a name for the sender (`TcpIp::can_resolve_address` 0x8aee00, called at 0x77b1be). A host with no reverse DNS name does not appear in the list.
7. The list is then sorted by game name (0x77b1d2..0x77b32a).
8. A BYE removes the entry with the same sender address (0x77b335..0x77b431). When no entry matches, the count goes up by one (0x77b448..0x77b457). This looks like a bug in the original. It can add a stale row. Unknown how often it shows.

The game list screen copies the 10 name slots to 0xdd6cd0 (0x81fafc..0x81fb06) and makes one button per game (0x81fb1e..0x81fb4b). The input handler rebuilds the screen when the count changes (0x81fc60..0x81fc76). A click on a game (button 0x17) copies its name into the Join field 0xdd70dc (0x81fd08..0x81fd63).

### Join by name or address

`Network::host_or_join_game` 0x77a9f0, join branch 0x77ab15:

1. Compare the Join text with each list name (0x77ab2a..0x77ab88). On a match, use that entry's address text.
2. With no match, use the Join text itself as the address.
3. Parse it with `sscanf("%d.%d.%d.%d")` (`parse_ipv4_address` 0x8ae0d0). A host name does not work, because the game has no `gethostbyname`.
4. Connect to port 6667 (`TcpIp::connect_to_host` 0x8ae640). On success, close the UDP sockets (0x77abcf) and set "connected".

## 3. The TCP game connection

### Host

`host_or_join_game` host branch (0x77aa2a..0x77ab12):

1. Set the host flag (0x77aa36).
2. Open one TCP listen socket per adapter with `TcpIp::open_listen_sockets` 0x8ae370. Set "connected" only if one opens (0x77aa41..0x77aa45).
3. Copy the game name to +0xc3a6c. Call `gethostname(+0xc3c60, 50)` at 0x77aa77.
4. Broadcast the ACK.

`open_listen_sockets` 0x8ae370 runs this for each adapter:

1. `socket(AF_INET, SOCK_STREAM, 0)` 0x8ae42d.
2. `SO_REUSEADDR 1` 0x8ae455, checked.
3. `ioctlsocket(FIONBIO, 1)` 0x8ae48d, checked.
4. `SO_LINGER {0, 0}` 0x8ae4d4, checked.
5. `SO_KEEPALIVE 1` 0x8ae50c, `SO_DEBUG 1` 0x8ae526, `SO_SNDBUF 15000` 0x8ae54c, `SO_RCVBUF 15000` 0x8ae56a. Not checked.
6. `bind` to the adapter address, port 6667 (0x8ae5bd), then `listen(s, 16)` (0x8ae5e9).

The host never listens on 127.0.0.1, because the address lookup skips it (section 5).

### Joiner

`connect_to_host` 0x8ae640:

1. `socket(AF_INET, SOCK_STREAM, 0)` 0x8ae6d1, `SO_REUSEADDR 1` 0x8ae705.
2. `connect` 0x8ae74e on a blocking socket. There is no timeout.
3. Only after `connect` returns: `ioctlsocket(FIONBIO, 1)` 0x8ae774, `SO_LINGER {0, 0}`, `SO_KEEPALIVE`, `SO_DEBUG`, `SO_SNDBUF 15000`, `SO_RCVBUF 15000` (0x8ae79e..0x8ae810).

### Framing

`Network::send_packet` 0x77ac30:

- A packet is a 4-byte length, little-endian, then the body. The length goes out as four separate 1-byte sends (0x77ad10, 0x77ad22, 0x77ad34, 0x77ad46), then the body in one send (0x77ad5a). So each packet is five `send` calls.
- A length of 0 or less shows an error box (0x77ac43..0x77acb1). A length of 50,000 or more is dropped with no message (0x77acb4).
- The host with slot 0 sends to all 16 slots. With slot n it sends to that slot only. A joiner always sends to its host connection (+0x4ba).
- The function checks only the body send. A result of -2 means "closed": it marks the slot free and calls the disconnect callback (0x77ad61..0x77ad80). A short send is not seen.

`TcpIp::send_to_connection` 0x8ae8d0:

- It calls `send` once. On `WSAEWOULDBLOCK` (10035, 0x8ae930) it calls `send` again in a tight loop until the result is not -1 (0x8ae92b..0x8ae964). It then returns the count of that one call, which can be short (0x8ae966).
- Any other error closes the socket and returns -2 (0x8ae97a..0x8ae9a5).

`Network::receive_packet` 0x77af20, TCP part (0x77b45e..0x77b61e), for each of the 16 slots:

1. State -4..-1: read the 4 length bytes one `recv` of 1 byte at a time (0x77b4a8, 0x77b4d9).
2. A length of 0 resets the state. Otherwise the state becomes the length.
3. Read the body with `recv(remaining)` into the slot's 50,000-byte buffer (0x77b52f, 0x77b55b). The length is not checked against 50,000.
4. When the body is complete, copy it to the caller's buffer, set the out slot to slot + 1 and return 1 (0x77b5be..0x77b61e). One call returns at most one packet.
5. `recv` -1 or -2 marks the slot free and calls the disconnect callback (0x77b4e9..0x77b508, 0x77b564..0x77b583).

`TcpIp::receive_from_connection` 0x8ae9c0: `recv` -1 with 10035 returns 0 (0x8aea07..0x8aea17). Any other error, or `recv` = 0 (peer closed), closes the socket and returns -2.

### Accept

In the host branch of `receive_packet`, if a slot is free, `TcpIp::accept_connection` 0x8ae840 is called once (0x77af5a). It runs `accept(s, NULL, NULL)` on each of the 20 listen sockets (0x8ae869) and takes the first. With no free table entry it closes the new socket (0x8ae89e). On success the slot state is set to -4 and the connect callback runs with the slot number 1..16 (0x77af6d..0x77af89).

### Packet types

The first byte of each packet body is an opcode. `Display::process_command_packet` 0x564080 switches on opcodes 0..26 through the table at 0x567a1c (0x564121..0x56412a). From the first calls of each case:

| Opcode | Case | What it does (from its first calls) |
|---|---|---|
| 0 | 0x564131 | `Display::reset`, then reads |
| 1 | 0x564d97 | Reads widget fields (`Guio::get_fields` 0x5ee4a0) |
| 2 | 0x564557 | Not decoded |
| 3 | 0x565e8c | `Display::draw_screen` |
| 4 | 0x565ec6 | Returns -2: quit |
| 5 | 0x565ed7 | Calls `Display::send_screen_section` 0x562370: a screen request |
| 6 | 0x565f84 | Not decoded |
| 7 | 0x567013 | Progress box and cursor |
| 8 | 0x56708c | Widget field change, with sub-op table 0x567a88 |
| 9, 10 | 0x567439, 0x5674aa | Play a sound file. The path goes through `display_localise_media_path` 0x5680f0 to the receiver's own files. |
| 11, 12 | 0x56762c, 0x56767e | Stop sounds, set volume |
| 13, 14 | 0x5676c5, 0x567702 | Sound started, sound finished |
| 15, 16 | 0x56772e, 0x56776b | Area selection, scroll row |
| 17..22 | 0x5677b8..0x567849 | Print job steps (`print_text_lines` 0x84c830) |
| 23..26 | 0x567882..0x567928 | Build the lines of a remote print job in memory (calloc, realloc, malloc, free) |

None of these opcodes carries a file, a save or database records.

Other packets I saw being built:

- 0x04 alone: sent by `ScreenManager::disconnect_network` 0x7fc690 to slot 0 (0x7fc738..0x7fc766).
- 0x0e plus one byte: the joiner sends this at the top of each loop pass when its sound player reports a slot (0x7fdcaa..0x7fddfa). Opcode 14 on the host is "sound finished".
- 0x13 and more: the joiner sends this when it leaves its loop (0x7fe2d9..). I did not read the rest of that packet.

### The join handshake and what the host sends

There is no hello packet and no version check. The order is:

1. The joiner connects. That is the whole join step on the joiner's side (0x77ab15..0x77abde).
2. The host's `receive_packet` accepts it and calls the connect callback `scrman_add_player_record` 0x7ff630 (set by `ScreenManager::start_network_session` 0x7ff57f..0x7ff590).
3. 0x7ff630 makes a screen channel for the new player with Display 1 (0x7ff6d0..0x7ff745). With that channel current, it calls the joined callback at 0xdd6bf0 (0x7ff74b..0x7ff764). That is `setup_on_network_player_joined` 0x838a80. The game name screen and the game list both set it (0x81f1bd..0x81f1cc, 0x81f8e8..0x81f8f7).
4. 0x838a80 opens a screen on the new channel. When the counts at +8 or +0xc of the human manager object 0xb63c98 are above 0 (0x838a80..0x838a95), it opens Manager Status (0x81ff10). Otherwise it goes to the add human manager flow (`setup_add_human_manager` 0x820d40), or first to the club squad screen when 0xdd7118 is set (0x838b6f..0x838b81). I did not read what +8 and +0xc count.
5. The host's screen loop `ScreenManager::monitor` 0x7fc7e0 builds each channel that needs a build (0x7fc93d..0x7fca61). For a remote channel it sends the screen with `Display::log_screen_sent` 0x5622a0 (0x7fca5c).
6. 0x5622a0 calls `send_screen_section` four times, with modes 0, 2, 1 and 3 (0x5622d8..0x56230d). It then formats `Screen sent: %ld bytes (%ld display, %d areas %ld, %d guios %ld, draw %ld )` into 0xde1f64 (0x562346).

So the first thing a human manager receives is the full screen that the host opened for them.

### The joiner's loop

`ScreenManager::run_network_client` 0x7fdc00 runs the joiner's whole session. The game list's Next button calls it after a good join (0x81fe16).

- Each pass calls `receive_packet` once (0x7fde79) and applies the packet to Display 1 (0x7fdeb3). Opcode 4 (result -2) ends the loop (0x7fdeba..0x7fdec5).
- A click is sent with `Display::send_widget_click` 0x568560 (0x7fe1df). Before that, if scrman +0x3838 is set, the loop draws "Contacting Server..." (0x7fe151..0x7fe1bb).
- After 30 seconds with no new screen it sends a keepalive (`Display::send_keepalive` 0x568470), based on `time()` 0x946ebb (0x7fe1f1..0x7fe24e).
- When a click send fails, 0x568560 shows "Network Error: Unable to send data ... The Server PC is has probably quit the game" (0x569456) and returns 0. The loop then ends (0x7fe1e4..0x7fe1e6).

## 4. Each WinSock call: sites, arguments, blocking

Every TCP and UDP socket that the game reads is non-blocking. The game polls each loop pass. It has no `select`, no receive timeout and no event messages. Two calls block: `connect`, and `send` while the socket is full.

| Call | Call sites | Arguments the game passes | Blocks or polls | What real sockets must do |
|---|---|---|---|---|
| `WSAStartup` | 0x8ae279, 0x8ae3b3, 0x8ae674, 0x8aeaad | version 0x101, WSADATA at TcpIp+0x328 | once | Return 0. Fill a version. I saw no read of WSADATA. |
| `WSACleanup` | 0x8ae329 | none | | Return 0. |
| `WSAGetLastError` | 0x8ae92b (after `send`), 0x8aea07 (after `recv`) | none | | Return 10035 for "would block". Return another non-zero code for every other error. |
| `gethostname` | 0x77aa77 | buffer +0xc3c60, 50 bytes | | Real name, cut to 49 bytes and a zero. |
| `gethostbyaddr` | 0x8aee0c | the 4 address bytes, 4, AF_INET | blocks (a DNS look-up) | Return a non-null entry for every IPv4 address with no look-up. The game only tests for null. |
| `socket` | 0x8ae42d, 0x8ae6d1, 0x8af2b3, 0x8af44b (TCP), 0x8aeb1a (UDP) | (2, 1, 0) or (2, 2, 17) | | A handle the game stores as a dword and compares with -1. Never reuse a closed handle at once (see `closesocket`). |
| `setsockopt` | listen 0x8ae455..0x8ae56a, connect 0x8ae705..0x8ae810, UDP 0x8aeb41..0x8aebb9, loopback test 0x8af2d2..0x8af3a6 and 0x8af474 | level 0xffff, options 4, 0x80, 8, 1, 0x1001, 0x1002, 0x20 | | Map the Windows numbers to host numbers (Linux uses other values). Apply SO_REUSEADDR, SO_BROADCAST and SO_KEEPALIVE. Accept SO_DEBUG and do nothing, because Linux refuses it to normal users. The game does not check that result. Leave the 15,000-byte buffers at the system size (see `send`). |
| `ioctlsocket` | 0x8ae48d, 0x8ae774, 0x8aeb72, 0x8af2fd | FIONBIO (0x8004667e), value 1 | | Set non-blocking. |
| `bind` | 0x8ae5bd (TCP), 0x8aec05 (UDP), 0x8af40f (loopback test) | adapter address or 127.0.0.1, port 6667 | | Convert the Windows sockaddr (no length byte) to the host layout. For UDP on Mac and Linux, see the note below the table. |
| `listen` | 0x8ae5e9, 0x8af42d | backlog 16 | | |
| `accept` | 0x8ae869 | (s, NULL, NULL), polled | polls | Return -1 and 10035 when nothing waits. Set the new socket non-blocking. Windows copies that from the listen socket, Linux does not. |
| `connect` | 0x8ae74e (join), 0x8af4e6 (loopback test) | blocking socket, IPv4 port 6667 | blocks, no timeout | Use a short timeout so that a wrong address does not freeze the game. Return 0 only when the connection is up. |
| `send` | 0x8ae91f, 0x8ae95a | whole buffer, flags 0 | busy loop on 10035 | Take the whole buffer every time. Never return a short count, because `send_packet` ignores it and the stream breaks. Queue what the system cannot take now. |
| `recv` | 0x8ae9fd | up to the bytes still needed, flags 0 | polls | 0 when the peer closed. -1 and 10035 when nothing waits. |
| `sendto` | 0x8aed1a (broadcast), 0x8aed5a (reply) | 20 or 119 bytes, flags 0 | | Needs SO_BROADCAST. |
| `recvfrom` | 0x8aedcd | 2,000 bytes, `from`, `from` length | polls | Ignore the input value of the length (it is not set at 0x77af90). Write 16. |
| `closesocket` | 0x8ae2c2, 0x8ae2ea, 0x8ae30e, 0x8ae35b and many in the open paths | | | A second close of the same handle must fail quietly (0x8aec17 then 0x8aec24). |
| `htons` | many | | | Already correct in `net.c`. |

UDP note for Mac and Linux: the game binds each UDP socket to the adapter's own address. Windows delivers subnet broadcasts to such a socket. Linux does not, and I expect macOS does not either. Slice 2 must test this. If it fails, the runtime binds the UDP socket to any address on port 6667 with SO_REUSEADDR and SO_REUSEPORT, and keeps the adapter address for its sends. The game also sets no SO_REUSEADDR on UDP, so a second game on one computer cannot bind port 6667. The game ignores that failure (0x77a84f), and that game then sees no game list.

## 5. The address lookup 0x8aee20

What it does:

1. Call `GetVersionExA` (0x8aee60).
2. On Windows 9x, run `winipcfg /batch ~net.out` (0x8aee77..0x8aee88). On NT 4, run `ip.exe` (0x8aee9e..0x8aeeaa).
3. On Windows 2000 and later, if the game is full screen, switch to an 800x600 window (0x8aeeb7..0x8aef0f). If that fails twice, show an error and end the program (0x8aef11..0x8aef7f). Then run `%windir%\system32\ipconfig > ~net.out` through `system` (0x8aef84..0x8aefee).
4. Open `~net.out` (0x8aeffb) and read it line by line, 200 bytes a line (0x8af030).
5. Count the lines that hold a colon. On 9x and NT 4, the line with count mod 4 = 2 is an address and mod 4 = 3 is a mask. On 2000 and later the divisor is 5 and the phases are 3 and 4 (0x8aee49..0x8aee4e, 0x8aef89..0x8aef8e, 0x8af082..0x8af096). This fits the Windows 2000 `ipconfig` layout. Extra lines in later Windows versions, such as IPv6 lines, move the count, and the game then reads the wrong lines. This is the root of the `~net.out` problems in the forum.
6. Skip an address that is 0 or `127.0.0.1` (0x8af13e..0x8af18c).
7. There is no limit on the entry count. More than 20 adapters would write past the table.

What it returns to its callers:

- In `*count_out`: the adapter count (zeroed at 0x8aee52, counted at 0x8af182).
- In `table`: 32 bytes per adapter. Bytes 0..15 are a sockaddr_in with family 2, port 0 and the address (0x8af114..0x8af130). Bytes 16..31 are a sockaddr_in with family 2, port 0 and the subnet broadcast (0x8af22c..0x8af267). Bytes 8..15 and 24..31 are not written.

The three callers:

| Caller | Table | Use |
|---|---|---|
| `TcpIp::open_listen_sockets` 0x8ae40a | stack | TCP listen socket on each address, port 6667 |
| `TcpIp::open_broadcast_sockets` 0x8aeb01 | TcpIp +8, count at +6 | UDP socket on each address. `send_datagram` sends to each broadcast address (0x8aecc9). |
| `Network::get_local_addresses` 0x77a980 | stack | Each address as dotted text in 20-byte records. `ScreenManager::get_local_addresses` 0x7fc7c0 passes them on. Its only caller is the Manager Status build 0x81ffaf. |

Smallest way to give the game its addresses: replace 0x8aee20 in the runtime with `RT_REPLACE(008aee20, ...)`. The port already does this for the print job 0x84c830 (`src/recomp_rt/win/print_pdf.c`). The calling convention is stdcall with two stack arguments, `ret 8` (0x8af28b). ECX holds the TcpIp pointer at two sites, and the function does not use it. The replacement:

1. Lists the IPv4 adapters that are up and not loopback (`getifaddrs` on Mac and Linux, `GetAdaptersAddresses` on Windows).
2. Writes at most 20 entries: address and `address | ~mask`, family 2, port 0, with the unset bytes set to 0.
3. Writes the count.

This removes the program start, the text file, the full-screen switch and the fatal exit. No game code changes. For online play, slice 4 may add an extra address here so that the host's game knows its relay address.

### The loopback test 0x8af290

Each `start_winsock` (0x8ae260) and each first socket open runs `TcpIp::test_loopback_connection` 0x8af290 (calls at 0x8ae292, 0x8ae3c4, 0x8ae689, 0x8aeabe). It binds a listen socket to 127.0.0.1:6667 (0x8af40f), listens, and connects a second, blocking socket to it (0x8af4e6). If any step fails, network play is off for that session: `start_network_session` 0x7ff59b..0x7ff5c9 drops the session, and the game list sends no REQ (0x81f902).

This matters for slice 2:

- With real sockets this test passes on a normal Mac.
- Port 6667 is the IRC port. Another program on 127.0.0.1:6667 stops network play.
- If the runtime lists 127.0.0.1 as an adapter (for a test with no network), the host listens on 127.0.0.1:6667. A second game on the same Mac then fails its own loopback test, because macOS and Linux refuse a second bind to a port with an active listener. Windows allows it with SO_REUSEADDR.

## 6. Does a joining human manager need the host's database or save?

Evidence:

1. Network Play is opened from the start screen (0x81bab0 pushes 0x81edd0 at 0x81cb99). Join Network Game (button 0x13) opens the game list directly (0x81f104..0x81f124). I saw no database load on this path.
2. After a good join, the joiner runs only `run_network_client` 0x7fdc00 until it leaves (0x81fe16..0x81fe2a). Its calls are the network, Display, input, graphics, the sound player and the message pump. I saw no database call.
3. The packet applier 0x564080 draws areas and widgets, plays sounds and prints. Widget text arrives as text (opcode 8). No opcode carries a file, a save or records (section 3).
4. Sound opcodes 9 and 10 open a sound file from the joiner's own install (0x5680f0). So the joiner needs the game's own media files, the same as any install.

Answer: by the code I read, a joining human manager needs neither the host's database nor the save. The host keeps both. Unknown: whether any screen of the host sends a picture or file name that only the host's database or update has. If such a name is sent and the joiner lacks the file, the result is unknown. Slice 2 can test it with a host on a different database.

Also unknown: whether a joiner with a different version can join. The original has no version check (section 3), so a mismatch shows only as wrong screens.

## 7. Join, holiday, passwords and host quit (spec slices 6 and 7)

### Drop and holiday (0x838c00)

1. A joiner's socket closes or fails. `receive_packet` or `send_packet` marks the slot free and calls the disconnect callback (0x77b4f3..0x77b508, 0x77ad6b..0x77ad80).
2. That callback is `scrman_remove_player_records` 0x7ff7b0. It acts only when 0xb77c80 is set. `setup_create_or_join_network_game` 0x8388e0 sets 0xb77c80 to 1 for the host and 0 for a joiner (0x838993..0x83899f).
3. It removes each screen channel of that player with `ScreenManager::remove_manager_record` 0x7ff990.
4. 0x7ff990 makes that channel current and calls the left callback at 0xdd6bf4 (0x7ffa1c..0x7ffab3). That is `setup_on_network_player_left` 0x838c00.
5. 0x838c00 takes the channel's manager row (+2 of the channel record, 0xb74c26).
   - A row in state 0 or 5 (still being made) is freed (0x838c48).
   - Otherwise, if the manager is not on holiday, it calls `HUMAN_MANAGER::set_on_holiday(1, 0)` (0x838c3e..0x838c41). A null manager means the current channel's manager (0x5fd2fe..0x5fd310).
   - It then jumps to `match_day_drop_viewer_and_finish_match` 0x6b6970.

So Away in the spec is the original's holiday. The network layer causes it only by closing the connection. Kick can work the same way: the runtime closes that human manager's socket, and the original's own leave path runs. The original has no Kick of its own in the code I read.

### Rejoin and manager passwords

1. A rejoining human manager connects like a new one (section 3). When the game runs, 0x838a80 opens Manager Status.
2. To take a club back, the password box input 0x8230b0 compares the typed text with the manager's password (0x82327f..0x8232c2).
3. It also accepts the fixed text `sportsinteractive` for any manager (0x8232cf..0x823308). This is a master password in the original. Any human manager who knows it can take any club. This bears on the spec's rule that the manager password protects a club in a public game. It is Max's decision whether the network layer can do anything about it. The rule says the original's password code runs unchanged.
4. On a match, it calls `set_computer_controlled(0, manager)` and `set_on_holiday(0, manager)` (0x823363..0x82337a). It then gives the manager to the current channel and sets both callbacks (0x82337f..0x8233dc).
5. When a different channel already holds that manager, it shows "<manager> has already resumed control." (0x823233).

### Restored game (0x8387e0)

`setup_assign_restored_network_managers` 0x8387e0 walks the 16 human manager rows (0x8387f0..0x8388b4):

- A human manager with no password, when 0xdd712c is 0, goes to the host's channel (0x838838..0x838886).
- A human manager with a password who is not on holiday becomes computer controlled (0x83888d..0x8388a5), to wait for its owner.
- A human manager on holiday sets the return value, which opens Manager Status.

### Host quit

1. `ScreenManager::end_network_session` 0x7ff5f0 calls `disconnect_network` 0x7fc690. That sends packet 0x04 to every human manager (0x7fc738..0x7fc766), then removes channels 1..15 and frees their sockets (0x7fc76b..0x7fc7ae).
2. Then `Network::~Network` 0x77a890 broadcasts BYE and closes all sockets.
3. Callers of `end_network_session`: `free_game_data` 0x82ff46, the start screen 0x81cb8c, the game name input 0x81f8a5 and the game list input 0x81fe2a. `~ScreenManager` 0x7fc567 calls `disconnect_network` directly.
4. On the joiner, opcode 4 returns -2 (0x565ec6), and `run_network_client` ends (0x7fdeba, 0x7fe253). The game list input then returns -2 (0x81fe31). I did not read what -2 does in the screen stack.
5. If the host crashes or the line drops, no 0x04 arrives. The joiner's slot closes when `recv` sees the close. The joiner learns of it only at its next click, through "Unable to send data ... The Server PC is has probably quit the game" (0x569456). A failed keepalive shows the same box from 0x568470 (0x568531), but the loop goes on.
6. I found no code that saves the host's game when it quits. The spec's "the host's game saves at once" is new behaviour. It needs a decision on where it lives, because the rule allows changes only in the network layer.

### Two candidate causes of the "Contacting Server" freeze

I found these by reading the code. No test has confirmed them yet, and slice 2 can test each one.

1. Short sends. `send_to_connection` can return a short count (0x8ae966), and `send_packet` checks only for -2. The rest of that packet is lost and the receiver's framing breaks. The receiver then waits for a body that never comes. A runtime that always takes the whole buffer removes this.
2. Dropped packets at screen-loop entry. `ScreenManager::monitor` 0x7fc7e0 reads every waiting packet and drops it before its loop starts (0x7fc901..0x7fc92a). A click that arrives then is lost, and the joiner waits for a reply. I did not read when `monitor` is entered.

Two smaller ones: a wrong address freezes the joiner inside the blocking `connect` (0x8ae74e), and a reverse DNS look-up that hangs freezes the game list (0x8aee0c).

## 8. Traffic estimate

Facts from the code:

- One packet carries at most 49,999 bytes plus the 4-byte length (0x77acb4). Each slot's receive buffer is 50,000 bytes (0x77b590).
- A screen for a remote human manager is four sections, each one `send_packet` or more (0x5622d8..0x56230d). `send_screen_section` 0x562370 has five `send_packet` sites..
- Each packet is five `send` calls (section 3).
- A joiner's click is one packet. While idle, the joiner sends one keepalive every 30 seconds (0x7fe212).
- A Display holds about 250 areas and 1,200 widgets at most (from the 2026-10-13 inventory of 0x55fba0 and 0x55f990, not read again here).

Estimate, with assumptions stated:

| Case | Bytes | Basis |
|---|---|---|
| Ceiling for one screen | about 200 KB | 4 sections x 50,000 |
| A normal screen | 5 to 50 KB, assumed | 100 to 400 widgets of about 50 bytes with text, plus areas. Not measured. |
| One remote human manager in menus | 0.5 to 5 KB/s from the host | 6 screens a minute at 5 to 50 KB |
| Clicks and keepalives from a joiner | under 0.1 KB/s | small packets |
| A match with commentary | unknown | widget changes (opcode 8) per commentary line |

The original already formats the size of each screen it sends, as "Screen sent: %ld bytes ..." (0x562346). It writes the text to 0xde1f64 and shows it nowhere I saw. Slice 2 can measure in `net.c` by counting bytes and packets per socket. That needs no game code.

For the relay on the free plan (20 incoming WebSocket messages count as one request): if the runtime forwarded each `send` call as one message, a packet would be five messages. A batch every 50 ms gives at most 20 messages a second per connection. A four-hour evening with a host and three human managers (4 connections) is then at most 4 x 20 x 14,400 / 20 = 57,600 requests, over half the 100,000 a day limit. The connections are idle for most of an evening, so I expect the real figure to be far lower. Slice 2 measures it.

## 9. What slice 2 must change, and what it must not touch

Change, all in the runtime (`src/recomp_rt/win/net.c` and new host files):

1. Correct the two ordinal labels: 12 is `ioctlsocket`, 51 is `gethostbyaddr`.
2. Implement all 18 calls on BSD sockets and WinSock 2, as in the table of section 4.
3. Convert sockaddr between the Windows layout and the host layout, and map option numbers and error codes. Return 10035 for "would block".
4. Make `send` take the whole buffer, queue what the system cannot take, and never return a short count.
5. Set every accepted socket non-blocking.
6. Ignore the `from` length input of `recvfrom`.
7. Answer `gethostbyaddr` for any IPv4 address with no DNS look-up.
8. Give `connect` a short timeout.
9. Make a second `closesocket` of a handle harmless, and do not reuse handle numbers at once.
10. Replace 0x8aee20 with `RT_REPLACE` and fill the adapter table directly (section 5).
11. Make LAN broadcasts arrive on Mac and Linux (section 4, UDP note). Test it first.
12. Decide how two games on one Mac meet, given the loopback test (section 5). Choices: the joiner uses the Mac's LAN address, or the runtime answers 0x8af290. The second needs Max's yes, because it replaces another function of the original.
13. Count bytes and packets per connection, for the traffic log.

Do not touch:

1. Any function of network.CPP (0x77a7d0..0x77b6e0), the screen manager, Display or the setup screens.
2. The tcpip.cpp functions other than 0x8aee20 (and 0x8af290 only after the decision in item 12).
3. The packet format: the 4-byte length, the opcodes, and the REQ, ACK and BYE layouts. Slice 4 must answer REQ with ACKs in the 119-byte layout and keep within the 9-game cap.
4. Port 6667.
5. The holiday code (0x838c00), the password box (0x8230b0) and the restored game code (0x8387e0).
6. Single-player paths. A network session object exists only after the network screens make one (0x81f1cc, 0x81f8f7). The same-save sets prove this.

Points for later slices that the original does not have: a version check, Kick, a ping, a host save on quit, and a reason for each failure. "The network game could not be created" and "could not be joined" (0x838a05, 0x838a3c) carry no reason.

## 10. A game list longer than 9 games

Added 2026-10-27 for Max's question: can the game list scroll past 9 games?

### Where the cap lives

The list lives inside the `Network` object. The allocation is 0xc3e54 bytes (0x7ff563). It holds two arrays of 10 slots of 50 bytes:

- Game names at +0xc3a6c.
- Address texts at +0xc3c60. This array ends exactly at the end of the object (0xc3c60 + 500 = 0xc3e54).

The count is at +0xc3a6a. The cap is `cmp ax, 9` at 0x77b0eb.

Every reader and writer:

| Function | Sites |
|---|---|
| `Network::Network` 0x77a7d0 | count reset 0x77a809 |
| `Network::~Network` 0x77a890 | host name in the BYE, 0x77a901 |
| `Network::host_or_join_game` 0x77a9f0 | host name and game name 0x77aa58, 0x77aa6d, 0x77aab4. Join look-up 0x77ab24, 0x77ab40, 0x77ab73, 0x77ab84. |
| `Network::receive_packet` 0x77af20 | ACK out 0x77b02b, 0x77b04b. Add 0x77b0e7..0x77b166. Sort 0x77b1ce..0x77b317, which reaches the address array through the name pointer with the fixed distance 500 (0x77b1ea, 0x77b1f0, 0x77b1f6, 0x77b29d, 0x77b2b2, 0x77b2d5). BYE removal 0x77b373..0x77b45a, with the same kind of fixed offsets at 0x77b3cc, 0x77b3f5 and 0x77b402. |
| `setup_build_select_network_game_screen` 0x81f8d0 | count reset 0x81f913. Copies 500 bytes (0x7d dwords) of names to the global 0xdd6cd0 (0x81faf2..0x81fb06), then makes one button per name from 0xdd6cd0 (0x81fb1a). |
| `setup_handle_select_network_game_input` 0x81fbd0 | count reset 0x81fc30, 0x81fde9. Count compare 0x81fc63. |

A click on a game copies the clicked button's own text (Display +0xba9de, 0x81fd14..0x81fd5e). It does not read the array.

### How the screen draws the list

The list is one area of 1 column and 6 rows per page (0x81fa9f..0x81fad4, flags 1). Each game is a button in its own row (0x81fb1e..0x81fb4b). Empty rows up to 6 get blank text (0x81fb5c..0x81fb8b). `Area::layout` 0x403350 adds a scrollbar when the rows used pass the rows per page (0x4034b3..0x4034c7). So the original list already scrolls for games 7 to 9. An area takes up to 1,200 widgets (`Area::add_widget` 0x403200, 0x40321b). No screen change is needed for 50 rows.

### The smallest change for about 50 games

It is all constant patches in the recompiler, in the same form as `tools/recomp/staff_reserve.py`. No `RT_REPLACE` is needed.

1. Grow the allocation at 0x7ff563 by 4,000 bytes (50 slots x 2 arrays x 50 bytes, less the 1,000 bytes there now).
2. Leave the names at +0xc3a6c, now 50 slots. Move the address array to +0xc4430, the new end of the names.
3. Change the address base at the 9 sites that use +0xc3c60, and at 0x77b1ea and 0x77b3cc. Change the fixed distance 500 at the 6 sort sites and the 2 BYE sites to 2,500.
4. Change the cap at 0x77b0eb from 9 to 50.
5. Point the screen copy (0x81fae7, 0x81fb02, 0x81fb1a) at a new 2,500-byte runtime buffer, and copy 625 dwords in place of 0x7d at 0x81fafc. The global 0xdd6cd0 cannot grow, because the game name text at 0xdd70a8 and the Join field at 0xdd70dc lie 984 bytes after it.

That is about 25 constant sites in 5 functions. The packets, the host logic and the join logic do not change. The host keeps its own name in slot 0 of the same arrays, so the host side moves with the patch.

Same-save risk: none that I found. The `Network` object exists only after a network screen makes it (0x81f1cc, 0x81f8f7). `ScreenManager::monitor` calls `receive_packet` only when the session pointer is set (0x7fc909, 0x7fcad3). The two screen functions are network screens. The four same-save sets would confirm this.

### Opinion

Medium. Each site is easy, but the sort and BYE code use derived offsets, so one missed site corrupts the list or the host name. Patching the array part needs a test that fills the list with 50 ACKs, sends some BYEs and joins the last entry. The risk to the host and join logic is low if every site is covered. The join uses the address text of the clicked name, so a wrong address offset joins the wrong host. A cheaper option keeps the cap at 9. In that case the runtime answers each REQ with the 9 games that best fit (for example, open seats first), and the rest wait for Refresh.

### As built in slice 4a (2026-10-27)

Every site of the table above was read again at its address and holds the constant named there. A scan of the whole code for the offsets +0xc3a60..+0xc3e60 and for 0xdd6cd0..0xdd70a8 found no other use of the list. The hits outside 0x77a7d0..0x77b5a0 and 0x81f8d0..0x81fe10 are bytes of other instructions, such as jump offsets. The derived offsets are +0xc3c92 (0x77b1ea, the address slot 1), +0xc3c2e (0x77b3cc, the address slot k - 1), -0x226 (0x77b1f6, 0x77b29d, 0x77b2d5: the distance 500 and one slot) and -0x1c2 (0x77b3f5: the distance 500 less one slot). The allocation instruction is `push 0xc3e54` at 0x7ff562. The global 0xdd6cd0 is 500 bytes: the next global starts at 0xdd6ec4.

Two points that the plan above missed:

1. The original has 10 slots for a cap of 9, so one slot is spare. A BYE from an address that is not in the list adds a row (0x77b448..0x77b457), and that row is the spare slot. With 50 slots and a cap of 50, that row would be past the arrays, and the BYE search and the buttons would read past them. So `tools/recomp/game_list.py` gives each array 51 slots: the names at +0xc3a6c, the addresses at +0xc4462, the object 0xc4e58 bytes (4,100 more), the distance 2,550. The screen copies 638 dwords (2,552 bytes) to a runtime buffer of that size (`rt_game_list_names`, `src/recomp_rt/win/game_list.c`). A second BYE from an unknown address still writes past the end, as in the original.
2. A game in the list is the sender address of its ACK (0x77b10d), and a second ACK from the same address is dropped (0x77b145..0x77b1b1). So a test with 50 games needs 50 sender addresses. `tests/smoke/game_list_run.py` gives a fake host 50 addresses in a Linux network namespace and runs the joiner under wine (`game_list_wine.sh`). With the original's 9 games, the run fails.

## 11. Slice 2 results: real sockets (2026-10-27)

Slice 2 built items 1 to 11 and 13 of section 9 in the runtime: `src/recomp_rt/win/net.c` (the 18 calls and `RT_REPLACE` of 0x8aee20), `src/recomp_rt/win/net_core.c` (handles, queues, counts) and `src/recomp_rt/host/net_posix.c` and `net_win32.c`. Item 12 stays as Max decided (Q32): two games on one Mac meet through the Mac's LAN address, and the loopback test 0x8af290 runs unchanged. `tools/recomp/widescreen.py` no longer patches 0x8aeed3..0x8aef00, because those instructions are inside the replaced 0x8aee20.

### What the tests showed

1. UDP on macOS (item 11), tested with a small C program before the build: a socket bound to the adapter address gets no subnet broadcast. Two sockets bound to `*:6667` with `SO_REUSEPORT` both get a broadcast, but a datagram sent to one address and port goes to the socket that bound first. So on one Mac the host's ACK reply (0x8aed5a) would go to the host itself. The runtime therefore gives each UDP socket of the game two host sockets: a shared one on `*:6667` that takes broadcasts, and one on the adapter address with a system port that sends. The reply goes back to that port. Each shared socket gets every broadcast, but the game has one UDP socket for each adapter (0x8aea80). So a shared socket keeps only datagrams from its adapter's subnet, as a socket bound to the adapter address does on Windows. Without that, the host answered one REQ once for each adapter, and a joiner that got one BYE for each adapter added a stale row for each copy after the first (0x77b448). The unit test `net_core` checks the reply path, the broadcast to two games and the subnet filter on macOS, and on Linux in an ubuntu 22.04 container.
2. The game list on this Mac shows the host's game three times, once for each adapter whose ACK arrives (192.168.68.106 and the two Parallels adapters 10.211.55.2 and 10.37.129.2). The rows have the same name, and the list sorts by name (0x77b1d2), so the first row can be an adapter that another computer cannot reach. The original does the same on a host with several adapters. Slice 4 can drop adapters that are not on the joiner's LAN.
3. Manager Status shows "Server IP Address" with all four addresses of this Mac (0x77a980 through the new 0x8aee20), the Tailscale address 100.110.189.44 included.
4. A wrong address no longer freezes the game: connect gives up after 4 s (item 8, unit test with 192.0.2.1).
5. Two games that start network play at the same moment on one computer collide in the loopback test: both bind 127.0.0.1:6667 for a moment, and one turns network play off (it showed screen 0x591b70). The two-game run starts its joiners 4 s apart. This cannot happen between two computers.

### Two-game run

`tests/smoke/network_run.py` starts a host and joiners with timed clicks. Each joiner types the host's LAN address in the Join field, clicks Next, and checks the host's screens against the host's own. `tests/smoke/network_wine.sh` runs the same with the Windows build under wine 6.0.3 in an ubuntu 22.04 linux/amd64 container, where the games meet on the container's address. Results, 2026-10-27:

| Run | Result |
|---|---|
| Mac, host and 1 joiner | PASS: the joiner's Enter Name, and its Enter Name after its typed names went to the host and back, equal the host's own (0.00 % of pixels differ) |
| Mac, host and 3 joiners (4 human managers) | PASS, the same for each joiner |
| Windows build under wine, host and 1 joiner | PASS, the same |
| Windows build under wine, host and 3 joiners | PASS, the same for each joiner |

### Finding: one click acts on several host screens

The opcode of a joiner's click packet is 6 (`Display::send_widget_click` 0x568560 writes it at 0x5685e4). In the client loop `run_network_client` 0x7fdc00, `Display::poll_input` 0x55f620 (called at 0x7fdf17) reports the widget under the mouse while the button is down. The loop sends a click when that widget differs from the last one it saw (0x7fdf2c), and a poll with no widget clears the last one (0x7fe1e8). Each new screen from the host clears the Display, so while the button stays down, every new screen gets the click again.

On this Mac each script click stayed down for many host screens. The joiner sent 113 click packets for 6 script clicks, and the host sent a full screen for each one. The click on Next to join also reached the host's Enter Name as Next, and the host answered "Names Are Blank". Under wine, where each new screen comes after the click has ended, the joiner sent 3 click packets in the whole run, and the join gave no "Names Are Blank".

On the original's WinSock, the length bytes went out at once and the body waited for the peer's delayed acknowledgement (up to 200 ms on Windows), so the next screen came after a normal click had ended. With real sockets on a LAN, and with the runtime's whole-packet sends and `TCP_NODELAY`, the next screen comes within milliseconds. A player's normal click then acts on two or more screens: Next on Enter Name can also click Next on Select Nationality.

This is the original's input code, not the network layer, so slice 2 left it.

### The fix: each press acts on one host screen (slice 2b, Q34, 2026-10-27)

How the game learns the button state. The window procedure sets the level flag 0xb67a34 at WM_LBUTTONDOWN (0x68d9bc) and clears it at WM_LBUTTONUP (0x68da03) (docs/research/2026-10-13-input-inventory.md). `poll_input` 0x55f620 first takes the one-shot results (hotkey, 0xb67a0c, 0xb67a1c), then calls the hit test of the Display's mode (0x560490 for mode 0, at 0x55f704). While 0xb67a34 is set, the hit test gives the widget under the pointer in every poll. The runtime already wraps 0x55f620 (`input_poll` in `src/recomp_rt/win/user32.c`) to pace held buttons in single-player screens. It numbers each press (`press_count`, one for each WM_LBUTTONDOWN that it posts).

The rule, in that wrapper. The wrapper notes the press, the widget and the count of host screens read so far each time a held poll gives a widget, at any call site. When the poll returns to 0x7fdf1c (the call at 0x7fdf17 in `run_network_client` 0x7fdc00) in a press that already gave a widget, it gives -1 in two cases:

- the widget is the same one, or
- a new host screen came since, that is a packet with opcode 0 (`Display::process_command_packet` 0x564080, case 0 at 0x564131 calls `Display::reset` 0x55f7c0). `net_core.c` counts these packets as the game reads them (`nc_resets_in`).

On -1 the client loop stores -1 as its last widget (0x7fe1e8) and sends nothing, as for a pointer over no widget. The next press gives its widget at once and goes to the host as before (0x7fe1df). The press of the join click gave Next on the game list before the client loop ran, and Enter Name came since, so that press no longer reaches the host's Enter Name.

Two details came from a log of each poll (2026-10-27) and from the two review agents:

- A press that ends a text entry gives the field in one poll and the widget under the pointer in the next poll of the same press, both at 0x7fdf17 (field 0xb, then Next 0x15 on Enter Name). The original sends both, because they differ (0x7fdf2c). A first version of the rule gave -1 for every later poll of a press and so lost Next. The test caught it (3 click packets, Enter Name stayed).
- The host sent 5 bytes within 2 ms of each click, before the next poll of the same press. A rule that counted every byte from the host also lost Next. Only opcode 0 counts.

What stays as before: every other caller of 0x55f620 (single-player screens and the joining game's own screens before the client loop), the held-button pacing, the scrollbar test 0x561f10, and the wheel and key clicks that hold 0xb67a34 for a count of passes (0xb67a3c, not "held" in the wrapper). A joiner cannot hold a +/- button to repeat it on a host screen. The original could not either, because the client loop sends a widget only when it differs from the last one (0x7fdf2c). A hotkey while the left button is down gives -1 in the client loop. That is a small case.

Test: `tests/smoke/network_run.py`. The joiner types its names, types one more letter in First Name, and clicks Next while that entry is open. The run checks:

- the joiner's first screen equals the host's Enter Name (no "Names Are Blank"),
- after Next the joiner's screen equals the host's own Select Nationality (not Select Team),
- each joiner sends exactly 4 click packets (First Name, Second Name, and First Name and Next from the last press).

On the slice 2 build it fails: the first screen was "Names Are Blank", Next led to Select Team, and the joiner sent 174 click packets. With the fix:

| Run | Result |
|---|---|
| Mac, host and 1 joiner | PASS, 4 click packets |
| Mac, host and 3 joiners | PASS, 4 click packets for each joiner |
| Windows build under wine, host and 1 joiner | PASS, 4 click packets |
| Windows build under wine, host and 3 joiners | PASS, 4 click packets for each joiner |

Under wine the repeat did not show before the fix either (section above), so the wine runs check that the fix does no harm there. The Mac runs prove the fix.

Not tested: a click through the relay (slice 5). The rule counts host screens as the game reads them in `nc_recv`, so the relay path must deliver the host's packets through `nc_recv` with the same framing. An opcode 0 that the host sends unasked, between the two polls of a press that ends a text entry, loses that press. The player then clicks again.

The same-save sets (start.sav, prematch.sav, a new game, yeovil.sav with `--days 7`, each against the slice 2 build as ref-build) pass: every run of each set is ok, and ref-build equals normal in all four.

### Traffic

The runtime counts bytes, send calls and packets (4-byte length and body) for each TCP connection, and packets and bytes for each opcode. `RECOMP_NET_LOG=<file>` writes a line when a connection opens, every 10 s while it moves bytes, and at its close with the counts by opcode.

The wine runs have no repeated clicks, so their figures are the real cost of a setup screen. Each run: the join, about 60 s on the setup screens with a few clicks, then the joiner quits.

| Human managers | Each connection, host to joiner | Each connection, joiner to host | Host total out |
|---|---|---|---|
| 2 (host and 1 joiner) | 2,102 bytes, 14 packets, 70 send calls | 256 bytes, 4 packets | 2.1 KB in 69 s |
| 4 (host and 3 joiners) | 2,098 to 2,340 bytes, 13 to 17 packets | 195 bytes, 3 packets | 6.5 KB in 62 s |

By opcode, for one connection: each screen is opcodes 0 (reset, about 40 bytes), 1 (widgets, about 500 bytes), 2 (about 160 bytes) and 3 (draw, 5 bytes), so about 700 bytes and 4 packets for Enter Name or Manager Status. One or two opcode 9 packets (play a sound, 5 bytes). Each click is opcode 6, 62 to 69 bytes with its length.

Before the fix, the Mac runs showed the cost of the repeated clicks: about 70 KB and 430 packets from the host to each joiner in 15 s (for example 4,854, 55,600, 15,557 and 565 bytes for opcodes 0 to 3, 113 times each), answering 113 click packets.

What the figures say:

- A setup screen costs about 700 bytes, far below the estimate of section 8 (5 to 50 KB). Screens with long lists (player search, squads) are not measured yet and will be larger.
- The host sends a screen only when a human manager clicks. A still screen costs nothing: the wine runs had 10 s periods with no bytes. The 30 s keepalive of section 3 adds one small packet.
- Each joiner's connection carries its own screens, so the host's traffic grows with the number of joiners and with their clicks.
- Each packet is 5 send calls of the game (section 3). The runtime holds the 4 length bytes and sends each packet in one system send.

Not measured: a match with commentary and the days of a running game, because the runs stop at the setup screens. Slice 4 or 9 should add a run that plays a match day with two human managers, after the click fix.

For the relay (slice 5): with batches of 50 ms, a click and its screen are one or two batches each way. The free-plan count then depends on the number of clicks in an evening, not on its length.

### Windows

The Windows build (mingw-w64, linked with ws2_32 and iphlpapi) passes the two-game run under wine with 1 and 3 joiners (table above). Under wine the game list showed one row, for the container's one adapter. The test on the Windows 11 virtual machine waits for the 0.6.0 gates (Q30).

### Same-save sets

With networking compiled in, the four same-save sets pass against `ref-builds/main-batchAX`: start.sav, prematch.sav, a new game and yeovil.sav with `--days 7`, each with every option run and ref-build equal to normal. The sets ran on the build before the review fixes, which changed only `net_core.c`, `net_win32.c`, `net.c` comments and the tests. start.sav passed again on the final build. ctest passes 50 of 50 on the Mac, with the new `net_core` test, and `net_core` passes on Linux in the container.

## 12. Slice 4: online games in the game list (2026-10-27)

Slice 4 adds the lobby under the game's own network code. No function of the original changes, and no packet layout changes. Files: `src/recomp_rt/win/net_lobby.c` (the glue), `lobby.c` (the lobby client and its thread), `lobby_http.c` (the one HTTP interface, where slice 5 adds the secure connection), `lobby_rows.c` (the rows of the game list), hooks and the join check in `net_core.c`, and threads in `host/net_posix.c` and `net_win32.c`.

### Where the runtime learns that a game hosts

From the datagrams that the game sends (section 2): the first `CM3 LIST SERVER ACK` (the start-up broadcast of `host_or_join_game` 0x77a9f0 at 0x77aa7c..0x77aafd, or a reply of `receive_packet` 0x77af20 at 0x77aff2..0x77b088) makes the game a host, with the computer name of bytes 19..68 and the game name of bytes 69..118. `CM3 LIST SERVER BYE` (`Network::~Network` 0x77a890, host flag only) closes the entry. A quit with no BYE closes it at exit.

### Where the runtime reads the season, the day and the clubs

All reads run in the hook of a network call of the game, on the game's own thread, at most once a second.

| Field | Where | Evidence |
|---|---|---|
| The data is loaded | `[0xadadfc]` and `[0xae23bc]` are set | `setup_create_or_join_network_game` 0x8388e0 tests `[0xadadfc]` at 0x8389c7 before `free_game_data`. A new network game is made at Select League(s) before its data loads, so the host registers only after. |
| Day | game date 0xae2c90: s16 day of the year, s16 year | `savename.c` (`win_game_date`), `picker.c` |
| Season | the year of the date from July, else the year before | rule of this slice |
| Human clubs | the last 16 rows of the staff table `[0xae23bc]`, 0x6e bytes a row, count `[0xae2364]`; a row is in use when byte +0x5f is not 0 | `HUMAN_MANAGER::clear_invalid_watched_fixtures` 0x5fe8d0 at 0x5fe8e3..0x5fe903; `setup_assign_restored_network_managers` 0x8387e0 at 0x8387f0..0x83880f |
| Club name | club at staff +0x39, short name at club +0x38, long name at club +4 | side bar 0x76300e and 0x763017; `savename.c` |
| Managers online | 1 + the open connections that the host accepted | `nc_peers` |
| Free seats | 16 less the larger of the rows in use and the managers online | the original's 16 slots (+0x4ba, section 1) |

### The game list

The rows come in the 119-byte ACK layout through the game's own `recvfrom` (0x8aedcd). One row for each host: a game is named by its computer name and game name. The first ACK from the subnet of the default route goes to the game at once; an ACK from another subnet waits 300 ms for a better one; later ACKs of the same game from another address are dropped. A BYE from an address with no row is dropped, so the original's extra row for such a BYE (0x77b448) no longer shows. On this Mac the host's three ACKs (192.168.68.106 and the two Parallels adapters) now give one row.

A lobby game that is not on the LAN is an ACK from an address in 0.66.0.0/16, which no host can have. Its text is the name with the season, the day and the managers online, for example `Sunday League  2003/04 14 Oct  3 online, password`, within the 49 bytes of the name slot. The join compares the Join text with the list names and uses that row's address (0x77ab2a..0x77ab88), and the runtime's connect sees the 0.66 address. Lobby rows go to the game 400 ms after the request at the earliest, so the LAN answers come first. The runtime counts the rows that the game holds and gives none past the game's cap of 9 (0x77b0eb), so a BYE never reaches a row that the game dropped. A host with two adapters in one subnet (Wi-Fi and Ethernet) also gives one row. So two computers with the same name that host games with the same name on one LAN show as one row: a rare case. Two lobby rows whose cut names give the same text get a number, because the join takes the first row with the Join text. The "full today" row starts with a space: the list sort is a plain byte compare (0x77b1fc..0x77b222), so it shows first. A LAN ACK that comes after a lobby row of the same game replaces it: a BYE from the 0.66 address, then the ACK. The lobby's hostName is `PC-` and a hash of the computer name, so the lobby shows no computer name and a joiner can match the two rows.

### The join check

The original has no version check (section 3). A joiner's connect to any address that is not 127.x sends `BOSMAN HELLO 1`, its version and a password. The host's runtime takes each new connection of the game's listen sockets and answers at every network call of the game, also while the game accepts no one (a full game, or between two polls of 0x77af5a): OK, the host's version, or a wrong password. A host that gives no answer, for example the original game on Windows, gets its own reason. The game's accept gets only connections that passed. The joiner waits at most 8 s, because a host that works out a day makes no network call for a few seconds; the original's connect waited with no limit. The game's bytes follow only after OK. A refused join shows the original's "Unable To Join" box (`confirm_push` 0x591840, called at 0x838a67 with the text formatted at 0x838a46) with the reason in place of "could not be joined". The loopback test 0x8af290 connects to 127.0.0.1 and gets no check.

### Test results (2026-10-27)

`tests/smoke/network_run.py --lobby` against the local server (`npx wrangler dev`): PASS. The lobby listed the game with name, version, database 3.9.68, season 2001, day 2001-07-14, 1 manager online, 15 free seats and no password, and listed nothing after the host quit. join1 (Online play on) saw one row for the host, clicked it and played Enter Name and Select Nationality as the host's own (0.00 % of pixels differ, 4 click packets). join2 (lobby rows only, version 0.0.1) saw `Bosman  2001/02 14 Jul  1 online` and the box "This network game runs Bosman Edition 0.5.0, and you have 0.0.1. Install Bosman Edition 0.5.0 to join it." join3 (Online play off) joined through the LAN, and its lobby saw no connection. join4 (a lobby that answers full_today) saw the "full today" row and its box. The plain two-game run passes as before.

Finding for the server (slice 3). The local Workers runtime drops a Durable Object after about 10 s with no request. The lobby keeps its games in memory only, so in the first run the host's heartbeat after 30 s got `unknown_game`. The game registered again with its code and token, as the contract says, and the game came back. Between the two, a joiner's list was empty. The test now asks for the list every 3 s. I don't know how long Cloudflare keeps an idle lobby object in memory: with one game and a heartbeat every 30 s the list may empty in the same way. The server could keep its games in Durable Object storage, or set an alarm. That is a slice 3 decision.

### Not built in slice 4

- The screens for the game password: they need Max's choice. The join check already carries the password, and the lobby keeps its flag.
- Joining a lobby game through the relay (slice 5): today a click on a lobby row gives a box that says so.
- The game id of a restored save (the one-copy rule, Q23): today each hosting has a new id.
- The lobby address of the release: empty until Max deploys the server. Tests set `RECOMP_LOBBY_URL`.

## 16. Slice 9: release tests, a match day and the free plan (2026-10-27)

Section number 16 leaves 13 to 15 for slices 5, 6 and 7, whose notes use 13 on their branches. Renumber at the merge.

### The release tests

The scripts of the four release tests (spec Q29) are in `tests/online/`, with `docs/TESTING.md` ("Online play") as their summary. They reuse the timed clicks of `tests/smoke/network_run.py`. What the runs on this Mac showed about the original's screens in a network game:

- Restore Network Game (0x81edd0, the button at 277, 242) asks for the game name, then shows Select File with the saves of the save folder, then Manager Status. A restored manager with no password shows "Not Playing". A click on its row gives "Resume control of Arsenal ?" (0x823420), and Yes takes it back. This matches `setup_assign_restored_network_managers` 0x8387e0 (section 7).
- A joiner of a restored game whose other managers are Away gets Manager Status (0x838a80). A click on its old manager's row, the password and Ok take the club back: "Player Two, Barnsley, Playing" on the joiner's Manager Status.
- A joiner of a game that runs gets Manager Status too, also when no manager is Away. Add Manager in its left bar opens Enter Name.
- On the host's news screen the left bar first shows a countdown (5, 4, 3, 2, 1) in place of Continue Game. In one probe the first click on the manager's name then opened no menu, and a click after Escape did. The scripts click, press Escape and click again, then wait for the menu.
- The host's Go on Holiday works as in single-player: the host is not asked "Exit the network game as you go on holiday ?" (0x7623da), which the holiday code asks only on a channel with a remote player (`[0xb74c24 + slot*0x300]`, docs/research/2026-10-15-holiday-next-match-inventory.md).
- The type step of a test script reads one word. A game name with a space ("00 S9") becomes "00". The scripts use one-word names.

A gap in release test 4. In the original a joiner who goes on holiday leaves the network game, and a joiner who stays must click Continue Game for each day. So `season_net_run.py` lets the three joiners play at the start of each leg (in July), then they drop and their clubs are Away, and the host plays the rest of the season on holiday as a network host. Every match day, tournament and season change runs in a network game with four human managers, three of them Away. No joiner is connected at the moment of a season change or a tournament match. A joiner that stays connected through a season would need a click loop such as the one of `match_day_run.py`, and a random event of its club (an offer for a player) stops that loop. Max decides whether the release test needs that (recommendation below).

### Cloudflare free plan figures (checked 2026-10-27)

From developers.cloudflare.com (Workers limits, Durable Objects pricing, limits and lifecycle). The pages were read through a summary tool, so each figure should be read once more on the page before a decision depends on it.

| Limit | Free plan |
|---|---|
| Worker requests | 100,000 a day, reset at 00:00 UTC |
| Durable Object requests | 100,000 a day. 20 incoming WebSocket messages bill as 1 request. Outgoing messages are free. A WebSocket connect is 1 request. An alarm run is 1 request. |
| Durable Object duration | 13,000 GB-s a day. An object bills 128 MB while it is in memory and cannot hibernate, so 13,000 GB-s is about 28.9 object-hours a day for all objects together. |
| Hibernation | An object with only hibernatable WebSockets hibernates after 10 s with no request or event and then bills no duration. An idle object is evicted after 70 to 140 s. |
| SQLite storage | 100,000 rows written a day, 5 million rows read a day, 5 GB. A delete counts as a row written. Each `setAlarm` is 1 row written. |

I don't know whether an object that serves only HTTP requests (the lobby) bills its 10 s of idle time after each request. The pricing page bills an object that is "idle in memory but unable to hibernate". The estimates below take the worst case.

### What the server uses, from its code

- A heartbeat every 30 s (`lobby.c`, 30 s from the lobby's answer): 2,880 Worker requests and 2,880 lobby requests a day for each open game.
- The lobby writes a game's row at most once a minute on a heartbeat (`STORE_SEEN_EVERY_MS`, `server/src/lobby.ts` lines 26 and 311), and once at register and close: about 1,442 rows a day for an open game.
- The relay writes its key and code at register, an alarm each hour (`CHECK_EVERY_MS`, `relay.ts` line 28: 24 rows and 24 requests a day), a row when the host connects, and a row and a delete for each join ticket. About 30 rows a day for an open game with a few joins.
- The game list asks the lobby at most once each 5 s while it is open (`LIST_CACHE_MS`, `net_lobby.c` line 68).
- The relay answers the runtime's "ping" without waking (`setWebSocketAutoResponse`, `relay.ts` line 44). The game's own keepalive (one packet each 30 s from each joiner, 0x7fe212, section 8) is game data, so it wakes the relay.

### Estimates

Always-on game (open all day, human managers only in the evening):

- Worker requests: 2,880 a day. With game list requests, about 30 games can stay open all day, as the server README says.
- Rows written: about 1,470 a day, so about 68 always-on games. The request limit comes first.
- Lobby duration, worst case: with 3 or more open games a heartbeat comes at least each 10 s, so the lobby never goes idle: 0.125 GB x 86,400 s = 10,800 GB-s a day, 83 % of the free 13,000. That leaves about 2,200 GB-s, or 4.9 relay-hours a day for all games together. With one open game the lobby is awake about 10 s of each 30 s, about 3,600 GB-s a day.
- Relay duration while no one plays: none, because the host's relay connection hibernates. A joiner who stays connected but idle sends its keepalive each 30 s, and each wakes the relay for at least 10 s: about 1,500 GB-s a day for each idle joiner, worst case.

Evening (a host and three human managers for 4 hours): see the match day below for the traffic. The relay stays awake while frames flow at least each 10 s. If it is awake the whole evening: 4 h x 450 GB-s = 1,800 GB-s, 14 % of a day's allowance. With the lobby's worst case, the free plan carries about one such evening a day.

What this means: the free plan's daily duration, not its requests, limits online play first, and the lobby's own awake time may take most of it. Recommendations for Max:

1. After the deploy, read the Durable Objects duration in the Cloudflare dashboard for one day with one or two open games. That gives the real lobby figure in place of the worst case.
2. If the lobby uses most of the duration: keep the game list in a cache, or move the lobby to a Worker that keeps the list in the Cache API, so that a heartbeat does not keep an object awake. That is a slice 3 change.
3. A paid Workers plan has a larger duration allowance. I did not check its price or figures. Q11 says the free plan.
