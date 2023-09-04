/************************************************************************
       -= SeeR - C-Scripting engine v 0.94a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad/seer/
File:seer.h
Desc:main header file
************************************************************************/
#ifndef __SEER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#define EXTERN extern
/* !!!!!!!!!!!!!!!! PAS de DLL !!!!!!!!!!!!!
#ifdef WIN32
//*****************************VC
#ifdef INTERNAL_FILE
 #define EXTERN __declspec(dllexport)
#else
 #define EXTERN __declspec(dllimport)
#endif

#else
//*****************************GCC
 #define EXTERN extern
#endif
*/


/**************************  SeeR INFO ***********************************/
#define SeeR_VERSION          94 /* SeeR_VERSION/100.0 */
#define SeeR_VERSION_STR      "0.94a"
#define SeeR_DATE             19990923    /* yyyymmdd */
#define SeeR_DATE_STR         "1999-09-23"

#ifndef __cplusplus
typedef enum bool {false,true} bool;
#endif
typedef char *scScript;

/*
   Here you can check some information on current state of SeeR.
   e.g.
   printf("%d lines compiled.\n",scSeeR_Get(scSeeR_LinesCompiled));
*/
/* Get Only */
#define scSeeR_Version            0 /* Returns SeeR_VERSION */

#define scSeeR_Build              1 /* Build version, returns: */
 #define scSeeR_Build_Release      1 /* SeeR may not debug, but is fastest */
 #define scSeeR_Build_Debug        2 /* if SeeR may generate debug files */

#define scSeeR_LinesCompiled      2 /* Number of lines compiled recently */
#define scSeeR_ParamCount         3 /* Size in bytes of parameters passed to imported function */

/* Set/Get */
#define scSeeR_SafeMode           1000 /* SG If SeeR restricts user from using some     */
                                       /* unsafe features (like pointers - in future)   */
                                       /*        UNIMPLEMENTED                          */
#define scSeeR_Debugging          1001 /* SG If debugging is on                         */
#define scSeeR_OpenDebug          1002 /* S  Open debug file, debugging on,             */
                                       /* parameter:filename                            */
#define scSeeR_CloseDebug         1003 /* S  Close file,Stop debugging                  */
#define scSeeR_Dispatcher         1004 /* SG function dispatcher,                       */
                                       /* parameters:                                   */
                                       /* int nr,scFunctionDispatcher fun               */
#define scSeeR_ProgressCallback   1005 /* SG progress callback                          */
                                       /* parameter:                                    */
                                       /* int (*fun)(int percent)                       */
#define scSeeR_ExprOpcode         1100 /* SG output statement separator opcode (EXPR)?  */
                                       /* false - is faster, but in multitasking script */
                                       /* may not be interrupted every X instructions   */
                                       /* parameter:                                    */
                                       /* bool EXPR                                     */
                                       /* default:false                                 */
#define scSeeR_StackSize          1101 /* SG set stack size in bytes                    */
                                       /* parameter:                                    */
                                       /* int stack_size_in_bytes                       */
                                       /* default:4000                                  */
int __cdecl scSeeR_Get(int,...); //these "..." reserved for future parameters
bool __cdecl scSeeR_Set(int,...); //returns true, if set successful

void __cdecl scInit_SeeR(void);

/**************************Data definitions*******************************/
EXTERN char scScript_Title[128],scScript_Author[128],*scScript_SeeR;
EXTERN int (__cdecl *scProgressCallback)(int percent);

typedef struct scFlags {
        int priority;
        int forkno;//0 if not a forked instance
        int speed;
        int paused;
} scFlags;

typedef struct scInstance {
        scScript code;
        char *data_stack;//here, first 256 ints are registers,then data, then stack
        scFlags flags;
        struct scInstance *forks,*next_fork,*forked;
        struct scInstance *next;
} scInstance;

typedef struct scScheduler{
         scInstance* first;
       } scScheduler;

//Script type info
typedef struct{
  int regDS;//for variable access-the address of global variables
  scInstance *ins;//the instance script is refering to
  int reserved;
} scScriptTypeHeader;

#define SC_MAX_FUNCTION_DISPATCHERS 16

#define scDispatch_Void         ((unsigned)1<<21)
#define scDispatch_Struct       ((unsigned)((1<<24)|(1<<23)|(1<<22)))
#define scDispatch_Script       ((unsigned)1<<25)
#define scDispatch_Double       ((unsigned)1<<26)
#define scDispatch_Member       ((unsigned)1<<27)
typedef void (*scFunctionDispatcher)(int *result,void *function,int *params,int paramcount,/*void *_this,*/unsigned int options);
void __cdecl scStandardDispatcher
        (int *result,void *function,int *params,int paramcount,unsigned int options);
void __cdecl scCDispatcher
        (int *result,void *function,int *params,int paramcount,unsigned int options);

/**************************Errors*****************************************/

enum {
  scOK=0,
  scErr_Parse=1,
  scErr_Declaration=2,
  scErr_CodeGen=3,
  scErr_Operand=4,
  scErr_Cast=5,
  scErr_Runtime=101,
  scErr_Violation=102,
  scErr_Internal=255
};

EXTERN int scErrorNo;
EXTERN char scErrorMsg[1024];
EXTERN char scErrorLine[1024];
//throw an error connected with scripts:
void __cdecl scRuntime_Error(char *er, ...);

/**************************Instances**************************************/
scInstance* __cdecl scCreate_Instance(scScript,char*,...);
void __cdecl scFree_Instance(scInstance *sci);
scInstance* __cdecl scGet_This();//returns scActual_Instance
bool __cdecl scAssignInstance(scScriptTypeHeader *script,scInstance *ins,char *typeinfo);
EXTERN scInstance *scActual_Instance;
EXTERN int scParamCount;

/**************************Compilation************************************/
scScript __cdecl scCompile_File(char *);
scScript __cdecl scCompile_Text(char *);
scScript __cdecl scLoad_Script(char *);

/**************************Running and conversation***********************/
int __cdecl scStart_Instance(scInstance *,...);// calls main(...) in script
int __cdecl scGet_Script_Size(scScript);
int __cdecl scCall_Instance(scInstance*,int address,...);
int __cdecl scVCall_Instance(scInstance* inst,int address,int paramc,int *params);
                                 //paramc is size of params in bytes!
int __cdecl scGet_Symbol(scInstance*,char *);

#define scVar(inst,adr,typ) (*((typ*)((inst)->data_stack+256*4+(adr))))
#define scNameVar(ins,name,typ) scVar(ins,scGet_Symbol(ins,name),typ)

/**************************Externals**************************************/

void __cdecl scAdd_Internal_Header(char *,char *);
void __cdecl scAdd_External_Symbol(char *,void *);
void __cdecl scAdd_Member_Symbol(char *name,...);
#define scAddExtSym(a) scAdd_External_Symbol(#a,&a);
void __cdecl scExport_Kernel();

/**************************MULTITASKING**********************************/
scScheduler* __cdecl scGet_Scheduler();
scScheduler* __cdecl scSet_Scheduler(scScheduler*);//returns previous scheduler,
                                   //if you pass NULL, it'll create a new one

int __cdecl scVLaunch_Instance(scInstance* inst,int spd,int address,int paramc,int *params);
                                        //paramc is size of params in bytes!
int __cdecl scLaunch_Instance(scInstance* inst,int spd,int address,...);
int __cdecl scContinue_Instances(void);//returns number of instances remaining to run

void __cdecl scPause_Instance(scInstance*,int);//pause if (int)
void __cdecl scKill_Instance(scInstance*);

scInstance* __cdecl scGet_Forked(scInstance*,int);
scInstance* __cdecl scFork_Instance(scInstance*);
void __cdecl scKill_Forked_Instances(scInstance*);

/****************Priorities**********************************************/
//can call all imported kernel API function
#define scrights_KERNEL 256
//cannot ---
#define scrights_USER     0
//when no instance is being run:
#define scrights_TOP MAXINT

/****************Status of instance**************************************/
#define scstatus_FREE    0
#define scstatus_RUNNING 1
#define scstatus_PAUSED  2
int scGet_Instance_Status(scInstance *);

/*****************Operating on Actual_Instance***************************/
scInstance* __cdecl scGet_Actual_Forked(int);
int __cdecl scGet_Forked_Status(int);
void __cdecl scKill_My_Forked_Instances();
void __cdecl scKill_Forked(int);

/*****************Titles and Author info ********************************/
char* __cdecl scGet_Title(scScript);
char* __cdecl scGet_Author(scScript);
char* __cdecl scGet_Instance_Title(scInstance*);

/*********************DEBUGGING******************************************/
void __cdecl scToggle_Debug(bool debon);
void __cdecl scOpen_Debug(char *filename);
void __cdecl scClose_Debug();//also turns debuggin off

/*********************PRIORITIES*****************************************/
EXTERN bool scIgnore_Rights;
void __cdecl scSet_Priority(scInstance*, int priority_level);
int __cdecl scGet_Priority(scInstance*);//if NULL returns scNONE
int __cdecl scGet_Actual_Priority();

//These macros are to check if an instance calling a function containing
//them has at least required priority - if not it'll set
//an error and return rets (if function returns void, rets should be `;')
#define scPriorityGuard(priority,rets) \
        if ((scGet_Actual_Priority()<priority)&&!scIgnore_Rights)\
        {\
        scRuntime_Error("Priority violation at instance `%s'",\
        scGet_Instance_Title(scActual_Instance));\
        scErrorNo=scErr_Violation; return rets;\
        }

#define scKernelOnly(rets) scPriorityGuard(scrights_KERNEL,rets);

#ifndef SC_STRING_UNUSED
#ifdef __cplusplus
struct scintString{
 char *cstr;
};
#else
typedef struct {
 char *cstr;
} scintString;
#endif

// string(char *x);
void __cdecl string_string_char(scintString* _this,char *x);

// string();
void __cdecl string_string(scintString* _this);

scintString* __cdecl string_operatorSET_int(scintString* _this,int x);
void __cdecl string_string_int(scintString* _this,int x);

//" c_import ~string();\n"
void __cdecl string_destroy(scintString* _this);

//" c_import string(string& f);\n"
void __cdecl string_copy(scintString* _this,scintString* f);
//" c_import string& operator=(string& s);\n"
// a=x=s; <=> a.operator=(x.operator=(s));
scintString* __cdecl string_operatorSET(scintString* _this,scintString* f);
//" c_import string& operator=(char * f);\n"
scintString* __cdecl string_operatorSET_char(scintString* _this,char* f);
//" c_import string operator+(string& x);\n"
// a=x+s; <=> a=x.operator+(s); <=> a.operator=(x.operator+(s));
scintString __cdecl string_operatorPLUS(scintString* _this,scintString* x);

//" c_import string operator+(char* x);\n"
scintString __cdecl string_operatorPLUS_char(scintString* _this,char *x);

//" c_import string operator()(int start,int len);\n"
scintString __cdecl string_sub(scintString* _this,int start,int len);

//" c_import bool operator<?>(string& x);\n"
int __cdecl string_cmp(scintString* _this,scintString* x);
//" c_import bool operator==(string& x);\n"
bool __cdecl string_eq(scintString* _this,scintString* x);
//" c_import bool operator==(char* x);\n"
bool __cdecl string_eq_char(scintString* _this,char* x);
//" c_import bool operator>(string& x);\n"
bool __cdecl string_gt(scintString* _this,scintString* x);
//" c_import bool operator<(string& x);\n"
bool __cdecl string_lt(scintString* _this,scintString* x);
//" c_import bool operator>=(string& x);\n"
bool __cdecl string_ge(scintString* _this,scintString* x);
//" c_import bool operator<=(string& x);\n"
bool __cdecl string_le(scintString* _this,scintString* x);

//" c_import void del(int start,int len);\n"
void __cdecl string_del(scintString* _this,int start,int len);

//" c_import void insert(int start,string& x);\n"
void __cdecl string_insert(scintString* _this,int start,scintString* x);

//index of first found occurance, or -1 if not found
int __cdecl string_find(scintString* _this,scintString* x);

//returns number of replaces
int __cdecl string_replace(scintString* _this,scintString* what,scintString* with,bool global);

//length of string
int __cdecl string_length(scintString* _this);

//" c_import void operator+=(string& x);\n"
// a+=x; <=> a.operator+=(x);
void __cdecl string_append(scintString* _this,scintString* x);
void __cdecl string_append_char(scintString* _this,char* x);
//" c_import int operator(int)();\n"//toint
int __cdecl string_toInt(scintString* _this);
//" c_import char operator[](int idx);\n"
char* __cdecl string_operatorIDX(scintString* _this,int idx);

void __cdecl string_reconstruct(scintString* _this);
#endif



#ifdef __cplusplus

/*********************C++ INTERFACE**************************************/
enum {scriptLoad,scriptCompileFile,scriptCompileText};

class seerScript{
 protected:
  scScript script;
 public:

  seerScript():script(NULL) {}

  seerScript(int a,char *text):script(NULL)
    {switch(a)
     {
      case scriptLoad       :script=scLoad_Script(text);break;
      case scriptCompileText:script=scCompile_Text(text);break;
      case scriptCompileFile:script=scCompile_File(text);break;
     }
    }

  ~seerScript()
    {/*free();*/}

  void __cdecl free(void)
    {if (script) ::free(script);}

  int __cdecl size(void) {return scGet_Script_Size(script);}

  char* __cdecl title(void) {return scGet_Title(script);}

  char* __cdecl author(void) {return scGet_Author(script);}

  scScript __cdecl get(void) {return script;}

  int __cdecl compileFile(char *filename)
      {script=scCompile_File(filename);return scErrorNo;}

  int __cdecl compileText(char *text)
      {script=scCompile_Text(text);return scErrorNo;}

  bool __cdecl load(char *filename)
      {script=scLoad_Script(filename);return script!=NULL;}

  bool __cdecl valid()
      {if (!script||!size()) return false;
       return true;
      }
};

#define seerVar(inst,adr,typ) (*(typ*)inst.var(adr))
#define seerNameVar(ins,name,typ) (*(typ*)inst.var(name))

class seerInstance{
 protected:
  scInstance *ins;
 public:

  seerInstance()
    {ins=NULL;}

  seerInstance(scScript sc)
    {ins=scCreate_Instance(sc,"");}

  seerInstance(scInstance* i)
    {ins=i;}
//  seerInstance(seerInstance& i) //make fork
//    {ins=scFork_Instance(i.ins);}

  void __cdecl free(void)
    {if (ins) scFree_Instance(ins);}

  ~seerInstance()
    {/*free();*/}


  scInstance *__cdecl get(void) {return ins;}

  int __cdecl symbol(char *s)
    {return scGet_Symbol(ins,s);}

  int __cdecl status(void)
    {return scGet_Instance_Status(ins);}

  char* __cdecl title()
    {return scGet_Instance_Title(ins);}

  int __cdecl create(seerScript& sc)
    {return (int)(ins=scCreate_Instance(sc.get(),""));}

  int __cdecl call(int addr,...)
    {
     va_list va;
     va_start(va,addr);
     return scVCall_Instance(ins,addr,-1,&(va_arg(va,int)));
            //-1 - default number of parameters
    }

  int __cdecl call(char *_symbol,...)
    {
     va_list va;
     va_start(va,_symbol);
     return scVCall_Instance(ins,symbol(_symbol),-1,&(va_arg(va,int)));
    }

  int __cdecl start()
    {return call("main");}

  int __cdecl vcall(int address,int paramc,int *params)
    {
     return scVCall_Instance(ins,address,paramc,params);
    }

  int __cdecl vlaunch(int spd,int address,int paramc,int *params)
    {
    return scVLaunch_Instance(ins,spd,address,paramc,params);
    }

  int __cdecl launch(int spd,int addr,...)
    {
     va_list va;
     va_start(va,addr);
     return scVLaunch_Instance(ins,spd,addr,-1,&(va_arg(va,int)));
    }

  int __cdecl launch(int spd,char *_symbol,...)
    {
     va_list va;
     va_start(va,_symbol);
     return scVLaunch_Instance(ins,spd,symbol(_symbol),-1,&(va_arg(va,int)));
    }

  void __cdecl pause(int p)
    {
    scPause_Instance(ins,p);
    }

  void __cdecl kill()
    {
    scKill_Instance(ins);
    }

  scInstance* __cdecl forked(int i)
    {return scGet_Forked(ins,i);}

  void __cdecl killForked()
    {scKill_Forked_Instances(ins);}

  void *__cdecl var(char *name)//returns pointer to variable
    {return var(symbol(name));}

  void *__cdecl var(int offset)//returns pointer to variable
    {if (offset==-1) return NULL;
     return (void*)(ins->data_stack+256*4+offset);
    }

  bool __cdecl valid()
    {return (ins!=NULL&&scGet_Script_Size(ins->code));}
};

class seerString:public scintString{
//TODO
public:
 char *cstr;

 seerString() {string_string(this);}
 seerString(char *x) {string_string_char(this,x);}
 seerString(int x) {string_string_int(this,x);}
 seerString(seerString& f) {string_copy(this,(seerString*)&f);}

 ~seerString() {string_destroy(this);}

 seerString& __cdecl operator=(int x) {return (seerString&)*string_operatorSET_int(this,x);}
 seerString& __cdecl operator=(seerString& s) {return (seerString&)*string_operatorSET(this,&s);}
 seerString& __cdecl operator=(char *s) {return (seerString&)*string_operatorSET_char(this,s);}
// a=x=s; <=> a.operator=(x.operator=(s));

 seerString __cdecl operator+(seerString& x) {return *(seerString*)&string_operatorPLUS(this,&x).cstr;}
 seerString __cdecl operator+(char *s) {return *(seerString*)&string_operatorPLUS_char(this,s).cstr;}
// a=x+s; <=> a=x.operator+(s); <=> a.operator=(x.operator+(s));

 seerString __cdecl sub(int start,int len) {return *(seerString*)&string_sub(this,start,len).cstr;}
 seerString __cdecl operator()(int start,int len) {return sub(start,len);}

 int __cdecl length() {return string_length(this);}

 int __cdecl cmp(seerString& x) {return string_cmp(this,&x);}
//  int operator<?>(seerString& x);
 bool __cdecl operator==(seerString& x) {return string_eq(this,&x);}
 bool __cdecl operator==(char *x) {return string_eq_char(this,x);}
 bool __cdecl operator>(seerString& x) {return string_gt(this,&x);}
 bool __cdecl operator<(seerString& x) {return string_lt(this,&x);}
 bool __cdecl operator>=(seerString& x) {return string_ge(this,&x);}
 bool __cdecl operator<=(seerString& x) {return string_le(this,&x);}
 void __cdecl operator+=(seerString& x) {string_append(this,&x);}
 void __cdecl operator+=(char* x) {string_append_char(this,x);}
// a+=x; <=> a.operator+=(x);

 void __cdecl del(int start,int len) {string_del(this,start,len);}
 void __cdecl insert(int start,seerString& x) {string_insert(this,start,&x);}
//index of first found occurance
 int __cdecl find(seerString& x) {return string_find(this,&x);}
//returns number of replaces
 int __cdecl replace(seerString& what,seerString& with,bool global=false) {return string_replace(this,&what,&with,global);}

 int __cdecl toInt() {return string_toInt(this);}
// int operator(int)();//toint
 char& __cdecl operator[](int idx) {return *string_operatorIDX(this,idx);}

 void __cdecl reconstruct() {string_reconstruct(this);}
};

}
#endif

#define __SEER_H__
#endif
