"""Club divisions of the start season for the template. Current = the template's last division (the season
before the template's own, which is the start season), fitted to the division sizes; England from FM's final tables
of the season before and the real play-off winners; real top divisions of other nations from the season settings.
Last division and last position = FM's final tables. Maps FM competitions to template club_comp ids by majority of
their clubs. Writes <work>/leagues.pkl and prints checks.
usage: python3 -I leagues.py <season start year> <template folder> <facts folder> <work folder>"""
import os,sys,struct,pickle,collections
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from cmdb import *
from seasons import settings
S_=settings(int(sys.argv[1]))
T=Folder(sys.argv[2]); X=os.path.join(sys.argv[3],''); W=sys.argv[4]
M=pickle.load(open(os.path.join(W,'match.pkl'),'rb'))
hist=pickle.load(open(X+'clubhist.pkl','rb')); tm=pickle.load(open(X+'teammap.pkl','rb'))
# Clubs of the final tables that closed before the start, mapped to the FM club that took their place.
_fc=pickle.load(open(X+'clubs.pkl','rb')); _fid={v['long']:k for k,v in _fc.items()}
_succ={_fid[a]:_fid[b] for a,b in getattr(S_,'SUCCESSORS',{}).items()}
tm={k:_succ.get(v,v) for k,v in tm.items()}
comps={struct.unpack_from('<i',r,0)[0]:cstr(r,4,51) for r in T.rows(12)}
trow={struct.unpack_from('<i',r,0)[0]:r for r in T.rows(0)}
g=lambda r,o:struct.unpack_from('<i',r,o)[0]
# Start-season division. The template's last division (+0x5b) is its start-season division, but it is right only for some
# divisions. A division is accepted when as many clubs have it as last division as it has slots (+0x57 count).
n57=collections.Counter(g(r,0x57) for r in trow.values())
n5b=collections.Counter(g(r,0x5b) for r in trow.values())
acc={d for d in n57 if d>=0 and n57[d]==n5b[d] and not comps[d].startswith('A ')}
# Start from the template's own leagues (+0x57). Swaps run only between accepted divisions and the division just below
# each (the most common template division of its relegated clubs). A club whose last division is accepted moves
# there; a club that must leave takes the slot that a mover freed, its own last division first. A nation whose
# divisions do not all keep their slot counts after the swaps plays its template leagues.
below=set()
for d in acc:
    below|={g(r,0x57) for r in trow.values() if g(r,0x5b)==d and g(r,0x57)!=d and g(r,0x57)>=0}
zone={d for d in acc|below if not comps[d].startswith('A ')}
cur={c:g(r,0x57) for c,r in trow.items()}
ENGLAND=S_.ENGLAND_NATION
inz=lambda r:g(r,0x57) in zone and g(r,0x5b) in zone and g(r,0x53)!=ENGLAND
free=collections.Counter()
movers=[c for c,r in trow.items() if inz(r) and g(r,0x5b) in acc and g(r,0x5b)!=g(r,0x57)]
for c in movers:
    free[cur[c]]+=1; cur[c]=g(trow[c],0x5b)
leavers=[c for c,r in trow.items() if inz(r) and g(r,0x57) in acc and g(r,0x5b)!=g(r,0x57) and g(r,0x5b) not in acc]
for c in leavers:
    b_=g(trow[c],0x5b)
    if free[b_]>0: free[b_]-=1; cur[c]=b_
dnat={}
for d in n57:
    if d>=0: dnat[d]=collections.Counter(g(r,0x53) for r in trow.values() if g(r,0x57)==d).most_common(1)[0][0]
badnat=set()
while True:
    ncur=collections.Counter(cur.values())
    badd={d for d in n57 if d>=0 and n57[d]!=ncur[d]}
    new={g(r,0x53) for c,r in trow.items() if cur[c] in badd or g(r,0x57) in badd}-badnat
    if not new: break
    badnat|=new
    for c,r in trow.items():
        if g(r,0x53) in badnat: cur[c]=g(r,0x57)
ncur=collections.Counter(cur.values())
bad=[(comps[d],n57[d],ncur[d]) for d in n57 if d>=0 and n57[d]!=ncur[d]]
tn={struct.unpack_from('<i',r,0)[0]:cstr(r,4,51) for r in T.rows(4)}
print('accepted divisions',len(acc),'movers',len(movers),'leavers',len(leavers))
print('nations on their template leagues (swaps did not close):',sorted(tn.get(n,'?') for n in badnat))
print('divisions with a wrong count',bad)
moved_n=collections.Counter(tn.get(g(r,0x53),'?') for c,r in trow.items() if cur[c]!=g(r,0x57))
print('clubs on another division than the template, by nation:',sorted(moved_n.items(),key=lambda kv:-kv[1])[:30])
rows06=[r for r in hist if r[2]==S_.START]   # final tables of the season before the start
vote=collections.defaultdict(collections.Counter)
last={}
for comp,team,yr,pos,*rest in rows06:
    fc=tm.get(team); t=M['clubmap'].get(fc)
    if t is None: continue
    vote[comp][g(trow[t],0x5b)]+=1
    last[t]=(comp,pos)
compmap={}
for comp,c in vote.items():
    top,n=c.most_common(1)[0]
    if top>=0 and n>=max(2,sum(c.values())//2): compmap[comp]=top
lastdiv={t:(compmap[c],pos) for t,(c,pos) in last.items() if c in compmap}
pickle.dump(dict(cur=cur,lastdiv=lastdiv,compmap=compmap),open(os.path.join(W,'leagues.pkl'),'wb'))
print('FM comps mapped',len(compmap),'clubs with a last-season place',len(lastdiv))
E=S_.ENGLAND_DIVISIONS                 # FM competition id -> template division name, top to bottom
tid={v:k for k,v in comps.items()}
tab=collections.defaultdict(list)
for comp,team,yr,pos,*rest in rows06:
    if comp in E or comp==S_.ENGLAND_FEEDER: tab[comp].append((pos,M['clubmap'].get(tm.get(team))))
for c in tab: tab[c].sort()
name=lambda t: cstr(trow[t],4,51) if t in trow else str(t)
def club(n):
    hit=[c for c,r in trow.items() if cstr(r,4,51)==n]
    if len(hit)!=1: sys.exit(f'leagues: {len(hit)} template clubs named {n}')
    return hit[0]
# Each division: the clubs that stayed, the clubs relegated from the division above, the clubs promoted from the
# division below (automatic places, then the play-off winner). Counts per FM competition id in S_.ENGLAND_MOVES.
want={}
order=list(E)+[S_.ENGLAND_FEEDER]
for i,c in enumerate(order[:-1]):
    up_auto,down=S_.ENGLAND_MOVES[c]                    # automatic promotions out of c, relegations out of c
    po=S_.PLAYOFF_WINNERS.get(c)                         # the play-off winner of c (promoted)
    n=len(tab[c])
    stay=[t for p,t in tab[c][up_auto:n-down] if po is None or name(t)!=po]
    above=[] if i==0 else [t for p,t in tab[order[i-1]][len(tab[order[i-1]])-S_.ENGLAND_MOVES[order[i-1]][1]:]]
    b=order[i+1]; b_auto=S_.ENGLAND_MOVES[b][0] if b in S_.ENGLAND_MOVES else S_.FEEDER_AUTO
    bpo=S_.PLAYOFF_WINNERS.get(b)
    below=[t for p,t in tab[b][:b_auto]]+([club(bpo)] if bpo else [])
    want[c]=above+stay+below
# England: the four Football League divisions from FM's final tables of the season before and the real promotions,
# relegations and play-off winners; clubs that leave them take the slots that the arrivals freed.
fl={tid[E[c]] for c in want}
freed=collections.Counter()
inset=set()
for c,lst in want.items():
    for t in lst:
        inset.add(t)
        if cur[t]!=tid[E[c]]:
            if not comps.get(cur[t],'').startswith('English') or cur[t] not in fl and comps.get(cur[t])!='English Conference': print('  into',E[c],'from',comps.get(cur[t]),name(t))
            freed[cur[t]]+=1; cur[t]=tid[E[c]]
left=[t for t,r in trow.items() if t not in inset and cur[t] in fl and g(r,0x53)==ENGLAND]
for t in sorted(left,key=lambda t: freed[g(trow[t],0x5b)]<=0):
    want_d=g(trow[t],0x5b)
    if freed[want_d]>0 and want_d not in fl:
        freed[want_d]-=1; cur[t]=want_d; continue
    d=next(d for d,n in freed.items() if n>0 and d not in fl); freed[d]-=1; cur[t]=d
    # repair: swap with a club in the wanted division whose last division is d
    y=next((y for y,r in trow.items() if cur[y]==want_d and g(r,0x5b)==d and g(r,0x53)==ENGLAND and y not in inset),None)
    if y is not None: cur[t],cur[y]=want_d,d
ncur=collections.Counter(cur.values())
print('England check: divisions with a wrong count',[(comps[d],n57[d],ncur[d]) for d in n57 if d>=0 and n57[d]!=ncur[d]])
pickle.dump(dict(cur=cur,lastdiv=lastdiv,compmap=compmap),open(os.path.join(W,'leagues.pkl'),'wb'))
for c,lst in want.items():
    tset={t for t,v in cur.items() if v==tid[E[c]]}
    ws=set(lst)
    print(E[c],'result',len(tset),'rule',len(ws),'only template',[name(t) for t in tset-ws],'only rule',[name(t) for t in ws-tset])

# The real top divisions of other nations in the start season (season settings), fitted to the slots of the template. Rank:
# last season's top-division clubs by position, then the others (promoted) by their second-tier position.
# Too many: the lowest ranked stay out. Too few: the best placed of last season's top-division clubs come in.
# The changes are swaps with the clubs that leave, so every division keeps its count. The lists run in their
# order in the settings (each nation top down); a club placed by one list is locked for the lists after it.
import unicodedata,re
REAL, PROMOTED = S_.REAL, S_.PROMOTED
TRANSLIT=str.maketrans({'ł':'l','Ł':'L','ø':'o','Ø':'O','æ':'ae','Æ':'AE','ß':'ss','đ':'d','Đ':'D','ı':'i','İ':'I','œ':'oe','þ':'th','ð':'d'})
def nm(s):
    s=unicodedata.normalize('NFKD',s.translate(TRANSLIT)).encode('ascii','ignore').decode().lower(); s=re.sub(r'[^a-z0-9 ]',' ',s); return ' '.join(s.split())
DROP={'fc','cf','sc','ac','sv','vfl','vfb','bk','if','fk','sk','k','r','s','v','c','f','a','d','de','club','sad','afc','1','04','05','96'}
import functools
@functools.lru_cache(maxsize=None)
def toks(s): return frozenset(w for w in nm(s).split() if w not in DROP)
nm=functools.lru_cache(maxsize=None)(nm)
invmap={v:k for k,v in M['clubmap'].items()}
fmrow={}
for comp,team,yr,pos,*rest in rows06:
    f=tm.get(team)
    if f is not None and (f not in fmrow or comp in compmap): fmrow[f]=(comp,pos)
def pick(names,pool):
    chosen=[]; miss=[]; pairs=[]
    for n in names:
        best=None;bs=0
        for c in pool:
            r=trow[c]
            for s_ in (cstr(r,4,51),cstr(r,0x38,26)):
                a,b=toks(n),toks(s_)
                if not a or not b: continue
                sc=len(a&b)/len(a|b) + (0.5 if nm(n)==nm(s_) else 0) - (0.3 if ({'b','ii','c'}&set(nm(s_).split())) else 0)
                if sc>bs: bs,best=sc,c
        if best is None or bs<0.5 or best in chosen: miss.append(f"{n} [best {cstr(trow[best],4,51) if best is not None else None} {bs:.2f}{' taken' if best in chosen else ''}]")
        else: chosen.append(best); pairs.append((n,best))
    pick.pairs=pairs
    return chosen,miss
natof_c=lambda c:g(trow[c],0x53)
@functools.lru_cache(maxsize=None)
def natof(d): return collections.Counter(g(r,0x53) for r in trow.values() if g(r,0x57)==d).most_common(1)[0][0]
# Clubs that a real list names. A division with more slots than real clubs fills its extra slots from other clubs,
# so it must not take a club that another (lower) list names.
named={dname:set(pick(names,[c for c,r in trow.items() if g(r,0x53)==natof(tid[dname])])[0]) for dname,names in REAL.items()}
# REAL lists run top-down per nation. England's league divisions rank above its REAL lists.
rk={tid[n]:i-len(E) for i,n in enumerate(E.values())}; rk.update({tid[k]:i for i,k in enumerate(REAL)})
order=list(REAL)
def fmdiv(c):
    row=fmrow.get(invmap.get(c)); return compmap.get(row[0]) if row else None
realrows=[]; report=[]; placed={}; carry=[]   # carry: named clubs that a list had no slot for; the next list of the nation takes them
locked=set()   # clubs that an earlier (higher) real list placed: later lists neither choose nor move them
for dname,names in REAL.items():
    d=tid[dname]; slots=n57[d]
    nat=natof(d)
    pool=[c for c,r in trow.items() if g(r,0x53)==nat and c not in locked]
    others=set().union(*(v for k,v in named.items() if k!=dname))
    chosen,miss=pick(names,pool); realrows+=[(dname,n,c) for n,c in pick.pairs]
    later={tid[k] for k in order[order.index(dname)+1:] if natof(tid[k])==nat}
    take=[c for c in carry if natof_c(c)==nat and c not in chosen and c not in locked and fmdiv(c) not in later]
    carry=[c for c in carry if c not in take]; chosen=take+chosen
    prom=PROMOTED.get(dname,[])
    def rank(c):
        f=invmap.get(c); row=fmrow.get(f)
        if row is None:
            sn_=cstr(trow[c],0x38,26); hit=[i for i,n in enumerate(prom) if toks(n)&toks(sn_)]
            return (2,hit[0]+1) if hit else (1,99)
        comp,pos=row; fd=compmap.get(comp)
        # Relegated clubs first, then the division's own clubs, then promoted clubs, each by last season's place.
        return (1 if fd==d else 0 if fd in rk and rk[fd]<rk[d] and natof(fd)==nat else 2, pos)
    sel=sorted(chosen,key=rank)[:slots]
    carry+=[c for c in chosen if c not in sel]; placed[dname]=sel
    if len(sel)<slots:
        extra=sorted([c for c,(dv,pos) in lastdiv.items() if dv==d and c not in sel and c in trow and c not in locked and c not in others],key=lambda c:lastdiv[c][1])
        sel+=extra[:slots-len(sel)]
    if len(sel)<slots:
        sel+=[c for c in pool if cur[c]==d and c not in sel and c not in others][:slots-len(sel)]
    if len(sel)<slots:   # more slots than real clubs: take unnamed clubs of the nation, highest current division first
        sel+=sorted([c for c in pool if cur[c]>=0 and c not in sel and c not in others],key=lambda c:rk.get(cur[c],999))[:slots-len(sel)]
    X=[c for c in sel if cur[c]!=d]; Y=[c for c in pool if cur[c]==d and c not in sel]
    if len(X)!=len(Y): report.append((dname,'not applied: in',len(X),'out',len(Y),'missing names',miss)); continue
    for x,y in zip(X,Y): cur[y]=cur[x]; cur[x]=d
    locked|=set(sel)
    report.append((dname,'applied',len(sel),'of',len(names),'real; in',[cstr(trow[c],0x38,26) for c in X],'missing names',miss))
for r_ in report: print(*r_)
print('real clubs with no slot, and where they go:',[(cstr(trow[c],0x38,26),comps.get(cur[c])) for c in carry])
ncur=collections.Counter(cur.values())
print('after overrides: divisions with a wrong count',[(comps[d],n57[d],ncur[d]) for d in n57 if d>=0 and n57[d]!=ncur[d]])
pickle.dump(dict(cur=cur,lastdiv=lastdiv,compmap=compmap,realrows=realrows),open(os.path.join(W,'leagues.pkl'),'wb'))
