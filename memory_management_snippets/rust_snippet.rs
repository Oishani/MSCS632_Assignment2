use std::fs;
use std::time::{Duration, Instant};
use std::thread;

// -------- Borrowing helpers --------
fn sum_borrowed(xs: &[i32]) -> i32 { xs.iter().sum() }          // immutable borrow
fn push_owned(xs: &mut Vec<i32>, x: i32) { xs.push(x); }         // mutable borrow

// -------- Tiny RSS "profiler" (works in Linux containers) --------
fn rss_kb() -> Option<usize> {
    // Parse VmRSS from /proc/self/status
    let status = fs::read_to_string("/proc/self/status").ok()?;
    for line in status.lines() {
        if line.starts_with("VmRSS:") {
            return line.split_whitespace().nth(1)?.parse::<usize>().ok();
        }
    }
    None
}

fn print_rss(label: &str, baseline: usize) {
    let now = rss_kb().unwrap_or(0);
    let delta = now as isize - baseline as isize;
    println!("{label}: {} kB (Δ from baseline: {delta} kB)", now);
}

fn main() {
    println!("=== Section A: Ownership & Borrowing ===");
    // Heap-allocated Vec + borrows
    let mut v = Vec::with_capacity(4);
    push_owned(&mut v, 10);            // &mut borrow
    push_owned(&mut v, 20);
    let total = sum_borrowed(&v);      // & borrow (read-only)
    println!("sum(v) = {total}");

    // Move semantics (no GC; deterministic drops)
    let s = String::from("hi"); // heap allocation
    let s2 = s;                 // MOVE: s -> s2 (s is now invalid)
    println!("moved string = {s2}");
    // ---- TOGGLE to show compile-time safety: uncomment one line, then Compile (not Run)
    // println!("use after move: {s}");           // ERROR: use of moved value
    // drop(v); println!("{:?}", v);               // ERROR: use after move (v is moved into drop)

    println!("\n=== Section B: Runtime memory profiling (RSS) ===");
    let baseline = rss_kb().unwrap_or(0);
    println!("RSS baseline: {baseline} kB");

    // ---- Scenario A: allocate and then free (make it obvious)
    let t = Instant::now();
    let mut buf = vec![0u8; 64 * 1024 * 1024]; // ~64 MB
    buf.fill(0xAA);                            // touch every page so it's actually committed
    print_rss("After alloc (A)", baseline);

    drop(buf);                                  // explicit early free (deterministic)
    thread::sleep(Duration::from_millis(50));   // tiny pause for the allocator/OS
    print_rss("After drop  (A)", baseline);
    println!("Elapsed (A): {:?}\n", t.elapsed());

    // ---- Scenario B: compare LEAK vs NO-LEAK using a scope
    let t2 = Instant::now();
    {
        let mut leak = vec![0u8; 64 * 1024 * 1024]; // another ~64 MB
        leak.fill(0xAA);                             // commit pages
        print_rss("After alloc (B)", baseline);

        // TOGGLE: keep this line to LEAK; comment it for the NO-LEAK run
        std::mem::forget(leak); // intentionally leak (Drop won't run)
    } // if not forgotten, leak would drop here

    thread::sleep(Duration::from_millis(50));       // small pause
    print_rss("After scope drop (B)", baseline);
    println!("Elapsed (B): {:?}", t2.elapsed());

    println!("\nNote: RSS may not fall exactly to baseline due to allocator/OS caching.");
}
