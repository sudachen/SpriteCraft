/////////////////////////////////////////////////////////////////
//                                                             //
// (c) 2004 by SkiF                                            //
//                                                             //
// Пример редактора карт для стратегий типа WarCraft,          //
// построенный на движке SpriteCraft Алексея Суда-Чена.        //
// Алгоритм матричного преобразования земли взят               //
// у Ивана Дышленко http://delphigfx.mastak.ru/2d/008/2d_8.htm //
// Карта полностью лежит в массиве aMap - надо - сохраняйте.   //
// Это не законченный продукт, а рабочий пример - кому что     //
// надо - не стесняйтесь, правьте!                             //
//                                                             //
/////////////////////////////////////////////////////////////////

Vector=new Array(25);
BasicTable=[
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
      9, 1, 6, 8, 4, 5, 6,15, 8, 9, 1,14, 4, 5,14,16,
      1, 1, 6,15, 5, 5, 6,15,15, 1, 1, 6, 5, 5, 6,16,
     10, 1, 2, 7,15, 5, 6, 7,15, 1,10, 2, 7,13, 6,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
      4, 5,15, 8, 4, 5,15,15, 8, 4, 5, 8, 4, 5, 8,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
      2, 6, 2, 7,15,15, 6, 7,15, 6, 2, 2, 7, 7, 6,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     12, 5, 7, 3, 4, 5,15, 7, 8, 4,13, 3,12,13, 8,16,
      3,15, 7, 3, 8,15,15, 7, 8, 8, 7, 3, 3, 7, 8,16,
     11, 6, 2, 3, 8,15, 6, 7, 8,14, 2,11, 3, 7,14,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16
];
EqualTable=[
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,10,16,16,12,13, 2,16, 3, 0,16,16,16,16,11, 7,
     16, 0,11,16,12,12,11, 3, 3, 0, 0,16,16,12,11, 3,
     16, 9,11,16,16, 4,14, 3,16,16, 0,16,16,12,16, 8,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,10,16,11, 0,10, 2, 2,11, 0,16,16, 0,10,11, 2,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16, 9, 0,12,16, 4, 9,12, 4,16, 0, 0,16,12, 9, 4,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,11, 9, 1,16, 2,14,16,16,16, 0,10,16, 6,
     16,16,10, 0, 9, 1, 1,10, 9,16,16, 0, 0,10, 9, 1,
     16,16,10,12,16,16, 1,13, 4,16,16, 0,16,16, 9, 5,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16
];
NotEqualTable=[
      9, 1, 6, 8, 4, 5, 6,15, 8, 9, 1,14, 4, 5,14,15,
      1, 1, 6,15, 5, 5, 6,15,15, 1, 1, 6, 5, 5, 6,15,
      1, 1, 6,15, 5, 5, 6,15,15, 1, 1, 6, 5, 5, 6,15,
      1, 1, 6,15, 5, 5, 6,15,15, 1, 1, 6, 5, 5, 6,15,
     10, 1, 2, 7, 5, 5, 6, 7,15, 1,10, 2,13,13, 6,15,
      4, 5,15, 8, 4, 5,15,15, 8, 4, 5, 8, 4, 5, 8,15,
     23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
     19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,
     24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
      2, 6, 2, 7,15,15, 6, 7,15, 6, 2, 2, 7, 7, 6,15,
      4, 5,15, 8, 4, 5,15,15, 8, 4, 5, 8, 4, 5, 8,15,
     18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
     16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
     20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
      2, 6, 2, 7,15,15, 6, 7,15, 6, 2, 2, 7, 7, 6,15,
      4, 5,15, 8, 4, 5,15,15, 8, 4, 5, 8, 4, 5, 8,15,
     22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,
     17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
     21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,
      2, 6, 2, 7,15,15, 6, 7,15, 6, 2, 2, 7, 7, 6,15,
     12, 5, 7, 3, 4, 5,15, 7, 8, 4,15, 3,12,13, 8,15,
      3,15, 7, 3, 8,15,15, 7, 8, 8, 7, 3, 3, 7, 8,15,
      3,15, 7, 3, 8,15,15, 7, 8, 8, 7, 3, 3, 7, 8,15,
      3,15, 7, 3, 8,15,15, 7, 8, 8, 7, 3, 3, 7, 8,15,
     11, 6, 2, 3,15,15, 6, 7, 8,14, 2,11, 3, 7,14,15
];

oE=new ActiveXObject("SpriteCraft.Engine")
oE.logging = true
oE.Init()
oE.sst="MapEditor.sst"
oE.title="2d map editor    (F4-fullscreen)"
oE.showFps=true
oE.nativeCursor=true
oE.vMode=3
oE.softRefresh=true

mapW=128; mapH=128
aMap=new Array(mapW*mapH)
for(i=0;i<aMap.length;i++) aMap[i]=0
wMap=oE.NewPredefinedTileMap("", "wMap", 99)
wMap.x=32; wMap.y=32
wmX=0; wmY=0; wmXmax=mapW-wMap.cols; wmYmax=mapH-wMap.rows
wmW=wMap.cols*wMap.tileWidth; wmH=wMap.rows*wMap.tileHeight

function Culculate(land){
  var i,BaseIndex,AdditionalIndex
  if (land==3){
    Vector[12]=15
    for (i=0;i<25;i++){
      BaseIndex=Math.floor(Vector[i]/16)
      AdditionalIndex=Vector[i]%16
      if (BasicTable[i*16+AdditionalIndex]!=16) Vector[i]=BaseIndex*16+BasicTable[i*16+AdditionalIndex];
    }
  } else {
    Vector[12]=land*16
    for (i=0;i<25;i++){
      BaseIndex=Math.floor(Vector[i]/16)
      AdditionalIndex=Vector[i]%16
      if (BaseIndex==land){
        if (EqualTable[i*16+AdditionalIndex]!=16) Vector[i]=BaseIndex*16+EqualTable[i*16+AdditionalIndex]
      } else {
        if (NotEqualTable[i*16+AdditionalIndex]<16) Vector[i]=BaseIndex*16+NotEqualTable[i*16+AdditionalIndex];
        else if (NotEqualTable[i*16+AdditionalIndex]>16) Vector[i]=land*16+NotEqualTable[i*16+AdditionalIndex]-16;
      }
    }
  }
}

function GetElement(x,y){
  if (x>=0 && x<mapW && y>=0 && y<mapH) return aMap[x+y*mapW];
  else return 0
}

function PutElement(x,y,element){
  if (x>=0 && x<mapW && y>=0 && y<mapH) aMap[x+y*mapW]=element
}

function DrawMap(){
  var i,j
  if (wmX<0) wmX=0
  if (wmY<0) wmY=0
  if (wmX>wmXmax) wmX=wmXmax
  if (wmY>wmYmax) wmY=wmYmax
  for(j=0;j<wMap.rows;j++) for(i=0;i<wMap.cols;i++) wMap.SetTile(i,j,aMap[mapW*(j+wmY)+i+wmX])
}

slL=oE.NewSprite_("ArrowL",98); slL.x=wMap.x;               slL.y=wMap.y+wmH
slR=oE.NewSprite_("ArrowL",98); slR.x=wMap.x+wmW-slR.width; slR.y=slL.y;            slR.xFlip=true
slH=oE.NewSprite_("ArrowM",98); slH.x=slL.x+slL.width;      slH.y=slL.y;            slH.xSize=slR.x-slH.x
slX=oE.NewSprite_("ArrowC",97); slX.x=slH.x;                slX.y=slH.y
slT=oE.NewSprite_("ArrowL",98); slT.x=wMap.x+wmW;           slT.y=wMap.y+slT.width; slT.degree=90; slT.xFlip=true
slB=oE.NewSprite_("ArrowL",98); slB.x=slT.x;                slB.y=wMap.y+wmH;       slB.degree=90
slV=oE.NewSprite_("ArrowM",98); slV.x=slB.x;                slV.y=slB.y-slB.width;  slV.degree=90; slV.xSize=slV.y-slT.y
slY=oE.NewSprite_("ArrowC",97); slY.x=slT.x;                slY.y=slT.y+slY.width;  slY.degree=90

slXmin=slX.x; slXmax=slXmin+slH.xSize-slX.width; slYmin=slY.y; slYmax=slYmin+slV.xSize-slY.width
slXdwn=false; slYdwn=false;

Lands=oE.NewPredefinedTileMap("", "Lands", 99)
Lands.x=slT.x+slT.width+32; Lands.y=oE.scrHeight/4
curLand=1
Border=oE.NewSprite_("Border",98); Border.x=Lands.x; Border.y=Lands.y+curLand*64

DrawMap()
Timer=0; TimeStep=100; wMoveX=0; wMoveY=0
do {
  oE.NextEvent()
  if (oE.EvtIsMouseDown() && slX.mouseOver) slXdwn=true
  if (oE.EvtIsMouseDown() && slY.mouseOver) slYdwn=true
  if (oE.EvtIsMouseUp()) {slXdwn=false; slYdwn=false}
  if (slXdwn && oE.EvtIsMouseMove()){
    slX.x=oE.mouseX; if (slX.x<slXmin) slX.x=slXmin; if(slX.x>slXmax) slX.x=slXmax
    wmX=Math.round((slX.x-slXmin)*wmXmax/(slXmax-slXmin)); DrawMap()
  }
  if (slYdwn && oE.EvtIsMouseMove() && oE.mouseY>=slYmin && oE.mouseY<=slYmax){
    slY.y=oE.mouseY; if (slY.y<slYmin) slY.y=slYmin; if(slY.y>slYmax) slY.y=slYmax
    wmY=Math.round((slY.y-slYmin)*wmYmax/(slYmax-slYmin)); DrawMap()
  }
  if (oE.mouseIn && oE.mouseLDown){
    if (slL.mouseOver) wMoveX=-1;
    else if (slR.mouseOver) wMoveX=1;
    else if (slX.mouseOver) wMoveX=0;
    else if (slH.mouseOver && oE.mouseX<slX.x) wMoveX=-10;
    else if (slH.mouseOver && oE.mouseX>(slX.x+slX.width)) wMoveX=10;
    else wMoveX=0;
    if (slT.mouseOver) wMoveY=-1;
    else if (slB.mouseOver) wMoveY=1;
    else if (slY.mouseOver) wMoveY=0;
    else if (slV.mouseOver && oE.mouseY<slY.y) wMoveY=-10;
    else if (slV.mouseOver && oE.mouseY>(slY.y+slY.width)) wMoveY=10;
    else wMoveY=0;
    if ((oE.frameTick-Timer)>TimeStep){
      Timer=oE.frameTick
      if (wMoveX!=0 && !slXdwn){ wmX+=wMoveX; DrawMap(); slX.x=slXmin+wmX*(slXmax-slXmin)/wmXmax}
      if (wMoveY!=0 && !slYdwn){ wmY+=wMoveY; DrawMap(); slY.y=slYmin+wmY*(slYmax-slYmin)/wmYmax}
      if (oE.mouseX>=wMap.x && oE.mouseX<slT.x && oE.mouseY>=wMap.y && oE.mouseY<slL.y){
        wI=Math.floor((oE.mouseX-wMap.x)/wMap.tileWidth); wJ=Math.floor((oE.mouseY-wMap.y)/wMap.tileHeight)
        for(j=0;j<=4;j++) for(i=0;i<=4;i++) Vector[j*5+i]=GetElement(wI+wmX-2+i,wJ+wmY-2+j)
        Culculate(curLand)
        for(j=0;j<=4;j++) for(i=0;i<=4;i++) PutElement(wI+wmX-2+i,wJ+wmY-2+j,Vector[j*5+i])
        DrawMap()
      }
    }
  }
  if (oE.EvtIsLClick() && oE.mouseX>=Lands.x && oE.mouseX<Lands.x+64 && oE.mouseY>=Lands.y && oE.mouseY<Lands.y+4*64){
    curLand=Math.floor((oE.mouseY-Lands.y)/64); Border.y=Lands.y+curLand*64
  }
  if (oE.IsKeyReleasedNow(154)) oE.fullscreen = !oE.fullscreen //Key_F4
  oE.DisplayEx(30)
} while (!oE.EvtIsESC() && !oE.EvtIsQuit())
