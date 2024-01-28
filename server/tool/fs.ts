import { cwd } from "process";
import { readFileSync, writeFileSync } from "fs";
import { join } from "path";
const { parse, stringify } = JSON;

const dbPath = join(cwd(), "db");

function ReadJSON<T>(path: string): T {
  const data = readFileSync(path);
  return parse(data.toString());
}

function WriteJSON<T>(path: string, data: T) {
  writeFileSync(path, stringify(data));
}

export { dbPath, ReadJSON, WriteJSON };
