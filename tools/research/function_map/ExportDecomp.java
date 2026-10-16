import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.app.cmd.function.CreateFunctionCmd;
import java.io.*;
import java.nio.file.*;
import java.util.*;

public class ExportDecomp extends GhidraScript {
    public void run() throws Exception {
        String[] args = getScriptArgs();
        List<String> lines = Files.readAllLines(Paths.get(args[0]));
        File out = new File(args[1]);
        FunctionManager fm = currentProgram.getFunctionManager();
        for (String l : lines) {
            Address a = toAddr(Long.parseLong(l.trim(), 16));
            if (fm.getFunctionAt(a) == null) {
                disassemble(a);
                new CreateFunctionCmd(a).applyTo(currentProgram, monitor);
            }
        }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        int n = 0;
        for (String l : lines) {
            String h = l.trim();
            File f = new File(out, h + ".c");
            if (f.exists()) continue;
            Function fn = fm.getFunctionAt(toAddr(Long.parseLong(h, 16)));
            String text;
            if (fn == null) text = "// no function\n";
            else {
                DecompileResults r = di.decompileFunction(fn, 60, monitor);
                text = (r != null && r.decompileCompleted()) ? r.getDecompiledFunction().getC() : "// decompile failed\n";
            }
            Files.write(f.toPath(), text.getBytes());
            if (++n % 500 == 0) println("decompiled " + n);
        }
        di.dispose();
    }
}
