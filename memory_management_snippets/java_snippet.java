import java.util.*;
import java.util.concurrent.TimeUnit;

public class GcSingle {
  static final boolean LEAK = true;      // set false for NO-LEAK run

  static final int MB_PER_CHUNK = 16;    // 16 MB per array
  static final int CHUNKS_B = 6;         // Scenario B allocates 6 chunks (≈96 MB)

  static long usedMB() {
    Runtime r = Runtime.getRuntime();
    return (r.totalMemory() - r.freeMemory()) / (1024 * 1024);
  }

  static void touch(byte[] a) { Arrays.fill(a, (byte)0xAA); } // commit pages

  static void forceGc() throws InterruptedException {
    // Stronger GC "nudge" for sandboxed runners
    for (int i = 0; i < 3; i++) { System.gc(); Thread.sleep(120); }
  }

  public static void main(String[] args) throws Exception {
    System.out.println("=== Java: GC demonstration ===");
    long base = usedMB();
    System.out.println("Heap baseline: " + base + " MB");

    // ---- Scenario A: allocate then drop ----
    long tA = System.nanoTime();
    byte[] a = new byte[MB_PER_CHUNK * 1024 * 1024];
    touch(a);
    System.out.println("After alloc (A): " + usedMB() + " MB");

    a = null;                 // drop reference
    forceGc();                // stronger than a single System.gc()
    long afterDropA = usedMB();
    System.out.println("After drop  (A): " + afterDropA + " MB");
    System.out.println("Elapsed (A): " +
        TimeUnit.NANOSECONDS.toMillis(System.nanoTime() - tA) + " ms\n");

    // ---- Scenario B: LEAK vs NO-LEAK ----
    long tB = System.nanoTime();
    List<byte[]> hold = new ArrayList<>();
    for (int i = 0; i < CHUNKS_B; i++) {
      byte[] b = new byte[MB_PER_CHUNK * 1024 * 1024];
      touch(b);
      if (LEAK) hold.add(b);   // keep references only in LEAK run
    }
    System.out.println("After alloc (B): " + usedMB() + " MB");

    if (!LEAK) hold.clear();   // drop references (NO-LEAK path)
    forceGc();

    long afterScopeB = usedMB();
    System.out.println("After scope drop (B): " + afterScopeB + " MB  (LEAK=" + LEAK + ")");
    System.out.println("Elapsed (B): " +
        TimeUnit.NANOSECONDS.toMillis(System.nanoTime() - tB) + " ms");

    System.out.println("\nNote: Java has no dangling pointers; 'leaks' are typically" +
        " retained references that keep objects reachable.");
  }
}