#include <RSGL/RSGL.hpp>
#include <fstream>

RSGL::window win("RSGL-Dino-Game",{500,500,500,500},{255,255,255});
bool running=true;


std::string to_string(int num){
      std::string output=std::to_string(num); int zeroes=4;
      for (int i=10; i < 100000; i*=10) if (num >= i) zeroes--;
      for (int i=0; i < zeroes; i++) output.insert(output.begin(),'0');
      return output;
}

std::string readFile(std::string file){
    char buff[1234]; // file buffer
    FILE*  fp = fopen (file.data(), "r"); // read file into a FILE*
    if (fp== NULL) return ""; // return nothing if file did not load properly for whatever reason
    fread(buff,sizeof(buff),1,fp);  std::string cur_string(buff); //put the file buffer into a std::string 
    fclose(fp); // close the file
    return cur_string; // return the file data
}

std::vector<RSGL::rect> Cactuses= {{win.r.width+80, (win.r.width/2-20),20,45}};

std::vector<RSGL::rect> cactuses;
std::vector<int> cactusesi;

int convert(int num){ num++; if (num==1 || num == 3) return 1; else if (num==2 || num == 4) return 2; return 3;  }

std::map<int, RSGL::point> memory; int lastDis; int distObj;
bool AI(){
      if (memory.find(distObj) == memory.end()) memory.insert({distObj,{0,0}});
      lastDis=distObj;
      return memory.at(distObj).y > memory.at(distObj).x;
}

int main(){
      srand(time(NULL));
      std::vector<RSGL::rect> dots;
      std::vector<RSGL::rect> clouds;
      int tick=-1,scoreTick=0, switchTick=0, speed=3, dinoY=0;
      int highScore=0; int score=0; 
      bool dead=false; bool jump=false, fall=false, duck=false, init=false, AIMode=false;  int restartFrame=0;
      
      for (int y=0; y < 2; y++){
            for (int x=(!y); x < 3; x++) Cactuses.insert(Cactuses.end(), { win.r.width+80,  (20*(y-1))+(win.r.width/2-20),  20*(y+1), 45 });
      } Cactuses.insert(Cactuses.end(),{win.r.width+80,(win.r.width/2+20),20,25});

      if (readFile("highScore") != "") highScore=std::stoi(readFile("highScore")); 
      std::string dinoImg="res/images/lonicidle.png";

      while(running){
            win.checkEvents();
            if (!cactuses.empty()) distObj=cactuses.at(0).x-40;
            if (AIMode && dead && restartFrame >= 200){ restartFrame=0; dead=false; cactuses={};tick=-1; score=0; dinoY=0; speed=3;}
            switch(win.event.type){
                 case RSGL::quit: running=false; break;
                 case RSGL::KeyReleased: if (!init && (win.event.key != "Meta_L" && win.event.key != "Meta_R" )) { restartFrame=0;  init=true; dinoImg="res/images/lonicwalk1.png"; } 
                        if (dead && (win.event.key != "Meta_L" && win.event.key != "Meta_R")) { init=false; dead=false; cactuses={};tick=-1; score=0; dinoY=0; speed=3;}
                        if ( !AIMode && (win.event.key == "space" || win.event.key == "Up") || (AIMode && AI(/*data*/) )) jump=true; 
                        if (win.event.key == "Meta_L" ||win.event.key == "Meta_R") AIMode=!AIMode; break;
                 default: break;
            }
            RSGL::drawRect({0,win.r.length/2,win.r.width,1},{0,0,0});

            if (clouds.empty() || (clouds.at(0).x < 0 && clouds.at(1).x < 0 && clouds.at(2).x < 0 && clouds.at(3).x < 0)){
                  clouds={}; for (int i=0; i < 4; i++){ 
                        bool notCollide=false; RSGL::rect nRect;
                        while (!notCollide){
                              notCollide=true;

                              nRect = {win.r.width+( ( rand() % 240 + 80) ), ( rand() % (win.r.length/2) - 100) ,rand() % 80 + 40 , 40};
                              for (auto& cloud : clouds) if (RSGL::RectCollideRect(nRect,cloud)) notCollide=false; 
                        }
                        clouds.insert(clouds.end(),nRect);
                  }
            }

            for (int i=0; i < dots.size(); i++) RSGL::drawRect(dots.at(i),{0,0,0});
            for (int i=0; i < clouds.size(); i++){ 
                  RSGL::drawImage("res/images/cloud.png",clouds.at(i)); if (!dead && init) clouds.at(i).x-=speed;
            }
            RSGL::drawText("HI "+to_string(highScore)+" "+to_string(score),{win.r.width-210,win.r.length/4,15},"res/fonts/PublicPixel.ttf", {0,0,0} );
            if (!dead){
                  if ((tick >= speed && init) || tick == -1){
                        dots={}; tick=0; 
                        for (int i=0; i < 50; i++) dots.insert(dots.end(),{( rand() % win.r.width),( rand() % 20 + (win.r.length/2)),(rand() % 4),2 });
                  } if (init){
                        if (scoreTick >= speed*2){ scoreTick=0; score++; }
                        if (highScore < score){
                              FILE* f = fopen("highScore","w+");
                              fwrite(std::to_string(score).c_str(),std::to_string(score).size(),1,f); fclose(f);
                              highScore=score; 
                        }
                        if (dinoY && fall) dinoY-=4;
                        if (jump && !fall){ dinoY+=4; if (dinoY==112) fall=true; } 
                        if (!dinoY && fall){ jump=false; fall=false;}
                        duck = win.isPressed("Down");
                        for (int y=0; y < cactuses.size(); y++){
                              for (int x=0; x < cactuses.size(); x++) if ( x != y && (RSGL::RectCollideRect(cactuses.at(x),cactuses.at(y)) || abs(abs(cactuses.at(x).x)-abs(cactuses.at(y).x)) < 100)) cactuses.erase(cactuses.begin()+x);
                        }
                        if (cactuses.empty() || (!cactuses.empty() && cactuses.at(0).x < 40)){ int r=rand() % (Cactuses.size()-1);
                              cactuses.insert(cactuses.end(), Cactuses.at(r));
                              cactusesi.insert(cactusesi.end(), r);
                        }
                  }
            } else if (dead){
                  dinoImg="res/images/lonicdead.png";
                  RSGL::drawText("G a m e  O v e r",{win.r.width/5,win.r.length/3,20},"res/fonts/PublicPixel.ttf", {0,0,0} );
                  RSGL::drawImage("res/images/restart.png",{win.r.width/2,win.r.length/3+30,30,30});
                  if (RSGL::RectCollidePoint({win.r.width/2,win.r.length/3+30,30,30},{win.event.mouse}) && win.event.type == RSGL::MouseButtonReleased){ dead=false; cactuses={};tick=-1; score=0; dinoY=0; speed=3;}
            }
            for (int i=0; i < cactuses.size() && init; i++){ 
                  if (!dead) cactuses.at(i).x-=speed;
                  if (cactuses.at(i).x < -40) cactuses.erase(cactuses.begin() + i);
                  if (RSGL::RectCollideRect(cactuses.at(i),{20, (20*duck)+(win.r.width/2-20)-dinoY,20,45/(duck+1)})){ 
                        dead=true;
                        if (AIMode) memory.at(lastDis).x--;
                  }
                  if (cactusesi.at(i) == Cactuses.size()-1) RSGL::drawImage("bird.png",cactuses.at(i) );
                  else RSGL::drawImage("res/images/cactus"+std::to_string(convert(cactusesi.at(i))) + ".png",cactuses.at(i) );
                  //RSGL::drawRect(cactuses.at(i),{0,0,0});
            }
            if (switchTick == 6 && init){
                if (!duck && !dead) dinoImg == "res/images/lonicwalk1.png" ? dinoImg="res/images/lonicwalk2.png" : dinoImg="res/images/lonicwalk1.png";
                if (duck && !dead) dinoImg == "res/images/lonicduck1.png" ? dinoImg="res/images/lonicduck2.png" : dinoImg="res/images/lonicduck1.png";
                switchTick=0;
            }
            RSGL::drawImage(dinoImg,{15, (20*duck)+(win.r.width/2-20)-dinoY,25,45/(duck+1)});
            std::string biases="";
            if (memory.find(AIMode) != memory.end()) biases = "\nJLazy Bias "+std::to_string(memory.at(AIMode).x)+"\nJump Bias "+std::to_string(memory.at(AIMode).y);
            RSGL::drawText("AI : " + std::to_string(AIMode) + "\nDist "+std::to_string(distObj) + biases,{400,400,15},"res/fonts/PublicPixel.ttf", {0,0,0} );
            win.clear();  tick++; scoreTick++; if (init) switchTick++; restartFrame++;
      } win.close(); 
}