#define ANativeActivity_onCreate ANativeActivity_onCreate_v672_legacy
#include "native_activity_host_v672.cpp"
#undef ANativeActivity_onCreate
#include <android/asset_manager.h>

namespace {
std::atomic<bool> run673{false};
std::atomic<int> page673{0};
std::atomic<int> touches673{0};
AInputQueue* input673=nullptr;
ANativeWindow* window673=nullptr;
std::thread loop673;
int w673=0,h673=0;
std::mutex data673;
std::string taskState673="ASSET MISSING";
std::string taskPreview673="NO TASK DATA";
std::string csvName673="ASSET MISSING";
std::string verifyTarget673="ASSET MISSING";
std::string sha673="PENDING";
int csvRows673=0,csvCols673=0,csvNum673=0;
int assetCount673=0;
enum {P673_HOME=0,P673_FILES=1,P673_TASKS=2,P673_VERIFY=3,P673_DATA=4};

bool readAsset673(AAssetManager*m,const char*name,std::string&out){
    if(!m)return false;
    AAsset*a=AAssetManager_open(m,name,AASSET_MODE_BUFFER);
    if(!a)return false;
    off_t n=AAsset_getLength(a);
    const void*p=AAsset_getBuffer(a);
    if(p&&n>=0)out.assign((const char*)p,(size_t)n);else out.clear();
    AAsset_close(a);
    return true;
}
void loadAssets673(AAssetManager*m){
    std::lock_guard<std::mutex>g(data673);
    assetCount673=0; csvRows673=csvCols673=csvNum673=0;
    std::string tasks,csv,verify;
    if(readAsset673(m,"v673/tasks.txt",tasks)){
        assetCount673++;
        std::stringstream ss(tasks); std::string line; int n=0;
        while(std::getline(ss,line)){if(n==0)taskPreview673=safe672(line,26);n++;}
        taskState673="FOUND "+std::to_string(n)+" LINES";
    }
    if(readAsset673(m,"v673/sample.csv",csv)){
        assetCount673++; csvName673="SAMPLE.CSV";
        std::stringstream fs(csv); std::string line;
        while(std::getline(fs,line)){
            csvRows673++; int cols=1; for(char c:line)if(c==',')cols++; csvCols673=std::max(csvCols673,cols);
            std::stringstream ls(line); std::string cell;
            while(std::getline(ls,cell,',')){char*end=nullptr;std::strtod(cell.c_str(),&end);if(end&&end!=cell.c_str()){while(*end&&std::isspace((unsigned char)*end))end++;if(*end==0)csvNum673++;}}
        }
    }
    if(readAsset673(m,"v673/verify.txt",verify)){
        assetCount673++; verifyTarget673="VERIFY.TXT";
        sha673=sha256_672((const unsigned char*)verify.data(),verify.size());
    }
}
const char* title673(int p){switch(p){case P673_FILES:return"PACKAGED FILES RO";case P673_TASKS:return"TASKS REAL DATA RO";case P673_VERIFY:return"REAL FILE SHA256";case P673_DATA:return"CSV REAL DATA TEST";default:return"V67.3 DATA TEST";}}
bool draw673(ANativeWindow*w){
    if(!w)return false;EGLDisplay d=eglGetDisplay(EGL_DEFAULT_DISPLAY);if(d==EGL_NO_DISPLAY||!eglInitialize(d,nullptr,nullptr))return false;
    const EGLint ca[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE};
    EGLConfig c=nullptr;EGLint n=0;if(!eglChooseConfig(d,ca,&c,1,&n)||n<1){eglTerminate(d);return false;}EGLint f=0;eglGetConfigAttrib(d,c,EGL_NATIVE_VISUAL_ID,&f);ANativeWindow_setBuffersGeometry(w,0,0,f);
    EGLSurface su=eglCreateWindowSurface(d,c,w,nullptr);const EGLint xa[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};EGLContext x=eglCreateContext(d,c,EGL_NO_CONTEXT,xa);
    if(su==EGL_NO_SURFACE||x==EGL_NO_CONTEXT||!eglMakeCurrent(d,su,su,x)){if(x!=EGL_NO_CONTEXT)eglDestroyContext(d,x);if(su!=EGL_NO_SURFACE)eglDestroySurface(d,su);eglTerminate(d);return false;}
    EGLint W=0,H=0;eglQuerySurface(d,su,EGL_WIDTH,&W);eglQuerySurface(d,su,EGL_HEIGHT,&H);w673=W;h673=H;glViewport(0,0,W,H);glClearColor(.025f,.07f,.15f,1);glClear(GL_COLOR_BUFFER_BIT);
    int s=W/310;if(s<3)s=3;if(s>5)s=5;int lx=5*s,rx=W/2+14*s,top=H-22*s,row=9*s,p=page673.load();
    text("ROBOT ADMIN AI OS",lx,top,s,.92f,.96f,1);text("V67.3 REAL DATA",rx,top,s,.6f,.75f,.95f);
    text("CPP CORE: READY",lx,top-row,s,.35f,.9f,.55f);text("SAFETY: LOCKED",rx,top-row,s,.35f,.9f,.55f);
    text("MODE: OFFLINE FIRST",lx,top-2*row,s,.75f,.85f,.95f);text("CONTROL: READ ONLY",rx,top-2*row,s,.75f,.85f,.95f);
    text("LOCKED BASE: V67.1",lx,top-3*row,s,.55f,.8f,.95f);text("WRITE: OFF",rx,top-3*row,s,.95f,.55f,.3f);
    text("TOUCH COUNT: "+std::to_string(touches673.load()),lx,top-4*row,s,.55f,.8f,.95f);text("DELETE: OFF",rx,top-4*row,s,.95f,.55f,.3f);
    int py=top-6*row;text(title673(p),lx,py,s,.35f,.9f,.55f);std::lock_guard<std::mutex>g(data673);
    if(p==P673_HOME){text("PACKAGED FILES: "+std::to_string(assetCount673),lx,py-row,s,.75f,.85f,.95f);text("TASKS: "+taskState673,lx,py-2*row,s,.75f,.85f,.95f);text("SHA256: READY",lx,py-3*row,s,.35f,.9f,.55f);text("CSV: "+csvName673,lx,py-4*row,s,.75f,.85f,.95f);}
    else if(p==P673_FILES){text("SCOPE: APK ASSETS",lx,py-row,s,.75f,.85f,.95f);text("COUNT: "+std::to_string(assetCount673),lx,py-2*row,s,.75f,.85f,.95f);text("F1: TASKS.TXT",lx,py-3*row,s,.75f,.85f,.95f);text("F2: SAMPLE.CSV",lx,py-4*row,s,.75f,.85f,.95f);text("F3: VERIFY.TXT",lx,py-5*row,s,.75f,.85f,.95f);}
    else if(p==P673_TASKS){text("TASK FILE: "+taskState673,lx,py-row,s,.75f,.85f,.95f);text("PREVIEW: "+taskPreview673,lx,py-2*row,s,.75f,.85f,.95f);text("CREATE: BLOCKED",lx,py-3*row,s,.95f,.55f,.3f);text("APPLY: BLOCKED",lx,py-4*row,s,.95f,.55f,.3f);}
    else if(p==P673_VERIFY){text("TARGET: "+verifyTarget673,lx,py-row,s,.75f,.85f,.95f);text("SHA256 ENGINE: READY",lx,py-2*row,s,.35f,.9f,.55f);text("HASH: "+sha673.substr(0,20),lx,py-3*row,s,.75f,.85f,.95f);text("RESTORE: BLOCKED",lx,py-4*row,s,.95f,.55f,.3f);}
    else{text("CSV: "+csvName673,lx,py-row,s,.75f,.85f,.95f);text("ROWS: "+std::to_string(csvRows673),lx,py-2*row,s,.75f,.85f,.95f);text("COLS: "+std::to_string(csvCols673),lx,py-3*row,s,.75f,.85f,.95f);text("NUM CELLS: "+std::to_string(csvNum673),lx,py-4*row,s,.75f,.85f,.95f);}
    int sy=py+2*row;text("RESTORE/APPLY: OFF",rx,sy,s,.95f,.55f,.3f);text("LIVE ORDERS: OFF",rx,sy-row,s,.95f,.55f,.3f);text("AUTO UPDATE: OFF",rx,sy-2*row,s,.95f,.55f,.3f);text("PROD GATE: CLOSED",rx,sy-3*row,s,.95f,.55f,.3f);
    int bh=14*s,by=4*s,gap=2*s,bw=(W-6*gap)/5;const char*bn[5]={"HOME","FILES","TASKS","VERIFY","DATA"};for(int i=0;i<5;i++){float gg=(p==i)?.48f:.27f;rect(gap+i*(bw+gap),by,bw,bh,.08f,gg,.28f);text(bn[i],gap+i*(bw+gap)+2*s,by+4*s,s,.92f,.96f,1);}eglSwapBuffers(d,su);eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);return true;
}
int input_cb673(int,int,void*){if(!input673)return 1;AInputEvent*e=nullptr;while(AInputQueue_getEvent(input673,&e)>=0){if(AInputQueue_preDispatchEvent(input673,e))continue;int handled=0;if(AInputEvent_getType(e)==AINPUT_EVENT_TYPE_MOTION&&AMotionEvent_getAction(e)==AMOTION_EVENT_ACTION_UP){float px=AMotionEvent_getX(e,0),py=(float)h673-AMotionEvent_getY(e,0);int s=w673/310;if(s<3)s=3;if(s>5)s=5;int gap=2*s,bw=(w673-6*gap)/5,bh=14*s;if(py>=4*s&&py<=4*s+bh){for(int i=0;i<5;i++){int bx=gap+i*(bw+gap);if(px>=bx&&px<=bx+bw){page673=i;touches673++;handled=1;break;}}}}else if(AInputEvent_getType(e)==AINPUT_EVENT_TYPE_KEY&&AKeyEvent_getAction(e)==AKEY_EVENT_ACTION_UP&&AKeyEvent_getKeyCode(e)==AKEYCODE_BACK){page673=P673_HOME;touches673++;handled=1;}AInputQueue_finishEvent(input673,e,handled);}return 1;}
void loop_fn673(){ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);while(run673){if(window673)draw673(window673);ALooper_pollOnce(120,nullptr,nullptr,nullptr);}}
void win_create673(ANativeActivity*,ANativeWindow*w){window673=w;}
void win_destroy673(ANativeActivity*,ANativeWindow*){window673=nullptr;}
void iq_create673(ANativeActivity*,AInputQueue*q){input673=q;AInputQueue_attachLooper(q,ALooper_forThread(),1,input_cb673,nullptr);}
void iq_destroy673(ANativeActivity*,AInputQueue*q){AInputQueue_detachLooper(q);if(input673==q)input673=nullptr;}
void destroy673(ANativeActivity*){run673=false;if(loop673.joinable())loop673.join();}
}
extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){loadAssets673(a->assetManager);a->callbacks->onNativeWindowCreated=win_create673;a->callbacks->onNativeWindowDestroyed=win_destroy673;a->callbacks->onInputQueueCreated=iq_create673;a->callbacks->onInputQueueDestroyed=iq_destroy673;a->callbacks->onDestroy=destroy673;run673=true;loop673=std::thread(loop_fn673);}
extern "C" const char* ra673_version(){return "67.3";}
extern "C" int ra673_core_ready(){return 1;}
extern "C" int ra673_asset_data_ready(){return assetCount673==3?1:0;}
extern "C" int ra673_write(){return -403;}
extern "C" int ra673_delete(){return -403;}
extern "C" int ra673_restore(){return -403;}
extern "C" int ra673_apply(){return -403;}
extern "C" int ra673_live_order(){return -403;}
extern "C" int ra673_auto_update(){return -403;}
