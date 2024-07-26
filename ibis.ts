const libName = `./libibis.so`;
// Open library and define exported symbols
const dylib = Deno.dlopen(libName, {
  prepareDevice: { parameters: ["buffer"], result: "pointer" },
  sendstring: { parameters: ["buffer", "pointer"], result: "void" },
  setdisplay: { parameters: ["buffer", "pointer"], result: "void" },
} as const);

const displayStringCommand = (text: string) => {
  const paddedString = text.padEnd(Math.ceil(text.length / 4) * 4, " ");
  return `zI${paddedString.length / 4}${paddedString}`;
};
const stringCommand = displayStringCommand("hallo");

const displayOtherCommand = (text: string) => {
  const paddedString = text.padEnd(Math.ceil(text.length / 4) * 4, " ");
  return `zA1zzzd`;
};
const command = displayOtherCommand("hallo");

const buffer = new TextEncoder().encode("/dev/ttyUSB0");
console.log("BUFFER => ", command);
const buffer2 = new TextEncoder().encode(command);
const tty = dylib.symbols.prepareDevice(buffer);
dylib.symbols.sendstring(buffer2, tty);

// const dataView = new Deno.UnsafePointerView(ret);
// console.log("RECEIVED IN DENO SPACE => ", dataView.getCString());
