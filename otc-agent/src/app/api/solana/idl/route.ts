import { NextResponse } from "next/server";
import { promises as fs } from "fs";
import path from "path";

export async function GET() {
  const idlPath = path.join(
    process.cwd(),
    "solana",
    "otc-program",
    "target",
    "idl",
    "otc.json",
  );
  const data = await fs.readFile(idlPath, "utf8");
  return new NextResponse(data, {
    status: 200,
    headers: { "content-type": "application/json" },
  });
}
