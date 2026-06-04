let Width = window.innerWidth/1;
let Height = window.innerHeight/1;

let Cube_Size = 70;

let FPS = 60;
let FrameDelay = 1000/FPS;

let points = [];
let edges = [];

let form = 0;

let fov = 500;

let pyramidPoints = [
    [0, 1, 0],
    [-1, -1, 1],
    [1, -1, 1],
    [1, -1, -1],
    [-1, -1, -1]
];

let pyramidEdges = [
    [0, 1],
    [0, 2],
    [0, 3],
    [0, 4],
    [1, 2],
    [2, 3],
    [3, 4],
    [4, 1]
];

let Cube_Points = [
    [ 1, 1, 1],
    [ 1, 1,-1],
    [ 1,-1, 1],
    [ -1,1, 1],
    [ 1, -1,-1],
    [ -1, 1,-1],
    [ -1,-1, 1],
    [ -1,-1,-1]
];

let Cube_Edges = [
    [0, 1],
    [0, 2],
    [0, 3],
    [1, 4],
    [1, 5],
    [2, 4],
    [2, 6],
    [3, 5],
    [3, 6],
    [4, 7],
    [5, 7],
    [6, 7]
];

const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');

canvas.width = Width;
canvas.height = Height;

let posX = 0, posY = 0, posZ = 1;
let angleX = 0, angleY = 0, angleZ = 0;

let PressedKeys = [];

function RotatePoint(x, y, degrees) {
    let radius = Math.sqrt(x*x + y*y);
    if(radius == 0){
        return [x, y];
    }
    let currentAngle = Math.atan2(y, x) * (180/Math.PI);
    let newAngle = currentAngle + degrees;
    let radiant = newAngle * (Math.PI/180);

    let newX = radius * Math.cos(radiant);
    let newY = radius * Math.sin(radiant);

    return [newX, newY];
}

function Get2DPointsFrom3D(x, y, z, startX, startY, startZ, angleX, angleY, angleZ) {
    let X, Y;

    startY = -startY;
    y = -y;

    x *= Cube_Size;
    y *= Cube_Size;
    z *= Cube_Size;

    let RotatedX = RotatePoint(y, z, angleX);
    y = RotatedX[0], z = RotatedX[1];

    let RotatedY = RotatePoint(z, x, angleY);
    z = RotatedY[0], x = RotatedY[1];

    let RotatedZ = RotatePoint(x, y, angleZ);
    x = RotatedZ[0], y = RotatedZ[1];

    let cameraZ = 3 * startZ;
    let depth = cameraZ + z;
    if(depth < 0.1) depth = 0.1;

    let transX = x-startX;
    let transY = y-startY;

    X = (fov * transX) / depth + Width/2;
    Y = (fov * transY) / depth + Height/2;

    return [X, Y];
}

function CheckPressedKeys() {
    if (PressedKeys['Shift']) {
        posY += 0.05;
    }
    if (PressedKeys[" "]) {
        posY -= 0.05;
    }
    if (PressedKeys["a"]){
        posX -= 0.05;
    }
    if (PressedKeys["d"]){
        posX += 0.05;
    }
    if (PressedKeys["w"]){
        posZ -= 0.01;
    }
    if (PressedKeys["s"]){
        posZ += 0.01;
    }
    if (PressedKeys["ArrowUp"]){
        angleX += 1;
    }
    if (PressedKeys["ArrowDown"]){
        angleX -= 1;
    }
    if (PressedKeys["ArrowLeft"]){
        angleY += 1;
    }
    if (PressedKeys["ArrowRight"]){
        angleY -= 1;
    }
    if (PressedKeys["q"]){
        angleZ += 1;
    }
    if (PressedKeys["e"]){
        angleZ -= 1;
    }
    if (PressedKeys["1"]){
        form = 0;
    }
    if (PressedKeys["2"]){
        form = 1;
    }
}

function CreateFrame() {
    CheckPressedKeys();

    ctx.fillStyle = 'black';
    ctx.fillRect(0, 0, Width, Height);
    let Points2D = [];

    if(form == 0){
        points = Cube_Points;
        edges = Cube_Edges;
    } else  if(form == 1){
        points = pyramidPoints;
        edges = pyramidEdges;
    }

    for(let i = 0; i < points.length; i++){
        Points2D.push(Get2DPointsFrom3D(points[i][0], points[i][1], points[i][2], posX*Cube_Size, posY*Cube_Size, posZ*Cube_Size, angleX, angleY, angleZ));
    }

    ctx.strokeStyle = '#0ff';
    ctx.lineWidth = 1;

    for(let i = 0; i < edges.length; i++){
        let Point1 = Points2D[edges[i][0]];
        let Point2 = Points2D[edges[i][1]];

        ctx.beginPath();
        ctx.moveTo(Point1[0], Point1[1]);
        ctx.lineTo(Point2[0], Point2[1]);
        ctx.stroke();

    }
} 

document.addEventListener('keydown', (event) => {
    if(event.key != "Shift" && event.key != " " && event.key != "ArrowUp" && event.key != "ArrowDown" && event.key != "ArrowLeft" && event.key != "ArrowRight"){
        PressedKeys[event.key.toLocaleLowerCase()] = true;
    }else{
        PressedKeys[event.key] = true;
    }
});
document.addEventListener('keyup', (event) => {
    if(event.key != "Shift" && event.key != " " && event.key != "ArrowUp" && event.key != "ArrowDown" && event.key != "ArrowLeft" && event.key != "ArrowRight"){
        PressedKeys[event.key.toLocaleLowerCase()] = false;
    }else{
        PressedKeys[event.key] = false;
    }
});

setInterval(CreateFrame, FrameDelay);