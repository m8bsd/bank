/**
 * A tiny async stand-in for C's scanf/getchar on stdin.
 *
 * It reads lazily (only when a value is actually needed) so that prompts are
 * always printed before the program blocks waiting for input, and it pauses the
 * stream between reads so the process can exit cleanly.
 */

const INT64_MAX = (1n << 63n) - 1n;
const INT64_MIN = -(1n << 63n);

function isSpace(c: string): boolean {
  return c === " " || c === "\t" || c === "\n" || c === "\v" || c === "\f" || c === "\r";
}

export class Scanner {
  private buf = "";
  private pos = 0;
  private hitEof = false;

  constructor(private readonly stream: NodeJS.ReadStream = process.stdin) {
    this.stream.setEncoding("utf8");
    this.stream.pause();
  }

  /** True once a read has run into end-of-input (like C's feof). */
  get eof(): boolean {
    return this.hitEof;
  }

  /** Release stdin so the process can exit. */
  close(): void {
    this.stream.pause();
  }

  private fill(): Promise<boolean> {
    if (this.hitEof) return Promise.resolve(false);
    if (this.stream.readableEnded) {
      this.hitEof = true;
      return Promise.resolve(false);
    }
    return new Promise<boolean>((resolve) => {
      const cleanup = () => {
        this.stream.off("data", onData);
        this.stream.off("end", onEnd);
        this.stream.off("error", onEnd);
        this.stream.pause();
      };
      const onData = (chunk: string | Buffer) => {
        cleanup();
        this.buf = this.buf.slice(this.pos) + chunk.toString();
        this.pos = 0;
        resolve(true);
      };
      const onEnd = () => {
        cleanup();
        this.hitEof = true;
        resolve(false);
      };
      this.stream.on("data", onData);
      this.stream.on("end", onEnd);
      this.stream.on("error", onEnd);
      this.stream.resume();
    });
  }

  private async peek(): Promise<string | null> {
    while (this.pos >= this.buf.length) {
      if (!(await this.fill())) return null;
    }
    return this.buf[this.pos];
  }

  private async skipWhitespace(): Promise<void> {
    for (;;) {
      const c = await this.peek();
      if (c === null || !isSpace(c)) return;
      this.pos++;
    }
  }

  /** Discard input up to and including the next newline (or EOF). */
  async flushLine(): Promise<void> {
    for (;;) {
      const c = await this.peek();
      if (c === null) return;
      this.pos++;
      if (c === "\n") return;
    }
  }

  /**
   * Like scanf("%ld"): skip whitespace, read an optional sign and digits.
   * Returns null if no integer could be read. Values outside the signed
   * 64-bit range are clamped, as glibc does.
   */
  async readInteger(): Promise<bigint | null> {
    await this.skipWhitespace();
    let sign = "";
    let c = await this.peek();
    if (c === "+" || c === "-") {
      sign = c;
      this.pos++;
      c = await this.peek();
    }
    let digits = "";
    while (c !== null && c >= "0" && c <= "9") {
      digits += c;
      this.pos++;
      c = await this.peek();
    }
    if (digits === "") return null;
    const value = BigInt(sign + digits);
    if (value > INT64_MAX) return INT64_MAX;
    if (value < INT64_MIN) return INT64_MIN;
    return value;
  }

  /**
   * Like scanf("%<max>s"): skip whitespace, then read up to `max` non-space
   * characters. Returns null on EOF.
   */
  async readWord(max: number): Promise<string | null> {
    await this.skipWhitespace();
    let word = "";
    while (word.length < max) {
      const c = await this.peek();
      if (c === null || isSpace(c)) break;
      word += c;
      this.pos++;
    }
    return word === "" ? null : word;
  }
}
