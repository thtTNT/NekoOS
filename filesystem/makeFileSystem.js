let fs = require("fs");

let imagePath = process.env.IMAGE_PATH
let imageSize = Number(process.env.IMAGE_SIZE);

console.log("Create empty image...");
let imageData = Buffer.alloc(imageSize);
fs.writeFileSync(imagePath, imageData);
console.log("Image create successfully!")