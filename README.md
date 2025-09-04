# MSCS632 – Assignment 2: Syntax, Semantics & Memory Management

## Repo Layout
- `original_snippets/` – clean baseline programs  
- `error_snippets/` – snippets with intentional **syntax errors**  
- `comparison_snippets/` – larger examples (types, scopes, closures)  
- `memory_management_snippets/` – **Rust / Java / C++ memory demos**  
- `README.md` – this file  
---

## Run the Memory Demos
> Numbers are approximate; focus on **direction** (alloc goes up, free goes down). Reduce sizes if the playground limits memory.

### Rust (ownership/borrowing + RSS)
1. Open in Rust Playground or Compiler Explorer → **Run**.  
2. Toggles:  
   - Uncomment the “use-after-move” line to show the **compile-time error**.  
   - Comment/Uncomment `std::mem::forget(leak)` to show **leak vs no-leak**.  
3. Output prints: baseline → **After alloc (A)** → **After drop (A)** → **After alloc (B)** → **After scope drop (B)**.

### Java (GC + retained-reference leak)
1. Open in an online Java runner → set `LEAK = true` → **Run** (heap stays high).  
2. Set `LEAK = false` → **Run** (heap drops after `forceGc()`).  

### C++ (manual `new[]/delete[]` + RSS)
1. Open in Compiler Explorer (clang) → **Run** with `LEAK = true` (RSS stays high).  
2. Set `LEAK = false` → **Run** (RSS drops after `delete[]`).  
---

## Notes
- Rust/C++ print **RSS** from `/proc/self/status`; Java prints **used heap** (`totalMemory - freeMemory`).  
- Small residuals after free are normal (allocator/JVM caching).  
- Prefer RAII in C++, note Rust’s compile-time safety, and Java’s GC + reachability model.
