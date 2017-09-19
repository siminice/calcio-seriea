#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.hh"

#define ROSTER_SIZE   22
#define MAX_EVENTS    40
#define MAX_CAT     1000
#define CAT_COLS      20
#define CAT_CELL      64
#define MAX_ROUNDS  40
#define MAX_TEAMS   24
#define NUM_ORD     10
#define MAX_RR       3

#define EV_GOAL      0
#define EV_OWNGOAL   1
#define EV_PKGOAL    2
#define EV_PKMISS    3
#define EV_YELLOW    4
#define EV_RED       5
#define EV_YELLOWRED 6
#define EV_PKSAVED   7
#define EV_COLS     30

#define DB_ROWS      600
#define DB_COLS       60
#define DB_CELL       30
#define DB_HOME          0
#define DB_AWAY          1
#define DB_SCORE         2
#define DB_DATE          3
#define DB_COMP          4
#define DB_ROUND         5
#define DB_VENUE         6
#define DB_ATTEND        7
#define DB_WEATHER       8
#define DB_ROSTER1       9
#define DB_COACH1       31
#define DB_ROSTER2      32
#define DB_COACH2       54
#define DB_REF          55
#define DB_ASIST1       56
#define DB_ASIST2       57
#define DB_OBSERV       58
#define DB_T1            8
#define DB_T2           31

const char *tnormal  = "\033[0m";
const char *thiyellow  = "\033[33;1m";

const char *months[] = {
"", "gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno",
    "luglio", "agosto", "settembre", "ottobre", "novembre", "dicembre"};

const char* evsymb = "'`\"/#!*\\";
int round = -1;
int day = 0;
int month = 0;
int season = 0;
int year = 0;
int city = -1;
int stad = -1;
int ref = -1;
int home = -1;
int away = -1;
int hid = -1;
int aid = -1;
int hx = -1;
int ax = -1;
int hpid[ROSTER_SIZE];
int apid[ROSTER_SIZE];
int hmin[ROSTER_SIZE];
int amin[ROSTER_SIZE];
int hco = -1;
int aco = -1;
int nev;
int evp[MAX_EVENTS];
int evt[MAX_EVENTS];
int evm[MAX_EVENTS];

Catalog *Pl = new Catalog();
Catalog *Co = new Catalog();
Catalog *Rf = new Catalog();
Locations *Loc = new Locations();
char cat[MAX_CAT][CAT_COLS][CAT_CELL];

char fns[256];
char lns[256];

int findMonth(char *s) {
  if (!s) return 0;
  for (int i=1; i<=12; i++) {
    if (strcmp(s, months[i])==0) return i;
  }
  return 0;
}

char* ltrim(char *s) {
  if (!s) return NULL;
  int i = 0;
  while (s[i]==' ' || s[i]=='\t') i++;
  return s+i;
}

void rtrim(char *s) {
  if (!s) return;
  int i = strlen(s) - 1;
  while (i>0 && (s[i]==' ' || s[i]=='\t' || s[i]=='\n')) i--;
  s[i+1] = 0;
}

int isUpper(char *s) {
  if (!s) return 0;
  int l = strlen(s);
  for (int i=0; i<l; i++) {
    if ((s[i]>='a' && s[i]<='z') || s[i]=='?') return 0;
  }
  return 1;
}

void toCamel(char *s) {
  if (!s) return;
  int l = strlen(s);
  for (int i=1; i<l; i++) {
    if (s[i]>='A' && s[i]<='Z') s[i] += 32;
  }
}

void sanitize(char *s) {
  // 012345678901
  // D&#39;Aquino
  char *pos;
  while ((pos=strstr(s, "&#39;"))!=NULL) {
    int l = strlen(pos); // 11
    pos[0] = 39; // D'#39;Aquino
    int k = 5;
    while (k<=l && pos[k]!=0) {
      pos[k-4] = pos[k]; // D'Aquino
      k++;
    }
    pos[k-4] = 0;
  }
}

void Tokenize(char *s) {
  char *tk[6];
  sanitize(s);
  tk[0] = strtok(s, " ");
  fns[0] = 0; lns[0] = 0;
  for (int j = 1; j<6; j++) tk[j] = strtok(NULL, " ");
  int ntf = 0;
  int ntl = 0;
  for (int j=0; j<6; j++) {
    if (tk[j]==NULL) continue;
    if (isUpper(tk[j])) {
      if (ntl>0) { strcat(lns, " "); }
      toCamel(tk[j]);
      strcat(lns, tk[j]);
      ntl++;
    } else {
      if (ntf>0) { strcat(fns, " "); }
      strcat(fns, tk[j]);
      ntf++;
    }
  }
}

char **club;
char **mnem;
int  NC, NT, ncat;

char ldb[DB_ROWS][DB_COLS][DB_CELL];
char edb[DB_ROWS][EV_COLS][DB_CELL];

struct alias {
  int   year;
  char *name;
  char *nick;
  alias(int, char*, char*);
  ~alias();
};

struct node {
  alias *data;
  node  *next;
  node(alias*, node*);
  ~node();
};

struct Aliases {
  node *list;
  Aliases();
  ~Aliases();
  void Append(alias *a);
  char* GetName(int y);
  char* GetNick(int y);
};

//-------------------------------------

alias::alias(int y, char *s, char *n) {
  year = y;
  name = (char*) malloc(strlen(s)+1);
  strcpy(name, s);
  if (n != NULL) {
    nick = (char*) malloc(strlen(n)+1);
    strcpy(nick, n);
  }
  else nick = NULL;
};

alias::~alias() {
  if (name) delete name;
  if (nick) delete nick;
};

node::node(alias *a, node *n) {
  data = a;
  next = n;
};

node::~node() {
  if (next) delete next;
  delete data;
};


Aliases::Aliases() {
  list = NULL;
};

Aliases::~Aliases() {
 delete list;
}

void Aliases::Append(alias *a) {
  node *n = (node*) malloc(sizeof(node));
  n->data = a;
  n->next = list;
  list = n;
};

char* Aliases::GetName(int y) {
  if (!list) return NULL;
  node *n = list;
  char *s = n->data->name;
  int x = n->data->year;
  while (y < x && n->next != NULL) {
    n = n->next;
    s = n->data->name;
    x = n->data->year;
  }
  return s;
}

char* Aliases::GetNick(int y) {
  if (!list) return NULL;
  node *n = list;
  char *s = n->data->nick;
  int x = n->data->year;
  while (y < x && n->next != NULL) {
    n = n->next;
    s = n->data->nick;
    x = n->data->year;
  }
  return s;
}

Aliases **L;

//--------------------------------------

int Load() {
  FILE *f;
  char s[2000], *tok[20], *ystr, *name, *nick;

  f = fopen("data/teams.dat", "rt");
  if (f==NULL) return 0;
  fscanf(f, "%d\n", &NC);
  club = new char*[NC];
  mnem = new char*[NC];
  L = new Aliases*[NC];
  for (int i=0; i<NC; i++) L[i] = new Aliases;
  for (int i=0; i<NC; i++) {
    fgets(s, 60, f);
    s[strlen(s)-1] = 0;
    mnem[i] = new char[16];
    club[i] = new char[32];
    memmove(mnem[i], s, 15); mnem[i][15] = 0;
    for (int j=0; j<30; j++) club[i][j] = 32;
    memmove(club[i], s+15, 30);
  }
  fclose(f);
  f = fopen("data/alias.dat", "rt");
  if (!f) return 0;
  for (int i=0; i<NC; i++) {
    if (feof(f)) continue;
    fgets(s, 2000, f);
    if (!s) continue;
    if (strlen(s) < 3) continue;
    s[strlen(s)-1] = 0;
    tok[0] = strtok(s, "*");
    for (int j=1; j<20; j++)
      tok[j] = strtok(NULL, "*");
    int k=0;
    while(tok[k]) {
      ystr = strtok(tok[k], " ");
      name = strtok(NULL, "~");
      nick = strtok(NULL, "@");
      int y = atoi(ystr);
      alias *a = new alias(y, name, nick);
      L[i]->Append(a);
//      fprintf(stderr, "Alias (%d,%s~%s) for %d.%s...\n", y, name, (nick!=NULL?nick:""), i, club[i]);
      k++;
    }
    s[0] = 0;
  }
  fclose(f);
  return 1;
}


char *NameOf(Aliases **L, int t, int y) {
  char *s = L[t]->GetName(y);
  if (!s) return club[t];
  return s;
}

char *NickOf(Aliases **L, int t, int y) {
  char *s = L[t]->GetNick(y);
  if (!s) return mnem[t];
  return s;
}

int id[MAX_TEAMS], res[MAX_RR][MAX_TEAMS][MAX_TEAMS], rnd[MAX_RR][MAX_TEAMS][MAX_TEAMS];

void LoadSeason(int year) {
  char filename[64];
  sprintf(filename, "data/a.%d", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: file %s not found.\n", filename);
    return;
  }
  char s[500], *tok[12];
  int i, j, k;
  int n, ppv, tbr, pr1, pr2, rel1, rel2, r, z;
  fscanf(f, "%d %d %d %d %d %d %d\n", &n, &ppv, &tbr, &pr1, &pr2, &rel1, &rel2);
  int numr = tbr/NUM_ORD + 1;
  NT = n;
  for (i=0; i<n; i++) {
    fgets(s, 200, f);
    tok[0] = strtok(s, " ");
    id[i] = atoi(tok[0]);
  }
  for (k=0; k<MAX_RR; k++)
    for (i=0; i<n; i++)
      for (j=0; j<n; j++) { rnd[k][i][j] = res[k][i][j] = -1; }

  for (k=0; k<numr; k++) {
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        fscanf(f, "%d %d", &r, &z);
        rnd[k][i][j] = r;
        res[k][i][j] = z;
      }
      fscanf(f, "\n");
    }
  }
  fclose(f);
}

int FindId(int t) {
  for (int i=0; i<NT; i++)
    if (id[i]==t) return i;
  return -1;
}

int FindNick(char *s, int season) {
  for (int i=0; i<NT; i++) {
    if (strcmp(s, NickOf(L, id[i], season))==0) return i;
  }
  return -1;
}

int Mid(int h, int a) {
  return (NT-1)*h + a - (h<a? 1 : 0);
}

void LoadDB(int year) {
  char filename[64], s[5000], *tk[DB_COLS];
  sprintf(filename, "db/lineups-%d.db", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    fgets(s, 5000, f);
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<DB_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<DB_COLS; j++) {
      if (tk[j]!=NULL) strcpy(ldb[i][j], tk[j]);
      else strcpy(ldb[i][j], " ");
    }
  }
  fclose(f);
}

void LoadEvents(int year) {
  char filename[64], s[5000], *tk[DB_COLS];
  sprintf(filename, "db/events-%d.db", year);
  FILE *f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", filename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    fgets(s, 5000, f);
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<EV_COLS; j++) tk[j]=strtok(NULL, ",\n");
    for (int j=0; j<EV_COLS; j++) {
      if (tk[j]!=NULL) strcpy(edb[i][j], tk[j]);
      else strcpy(edb[i][j], " ");
    }
  }
  fclose(f);
}

void SaveDB(int year) {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "db/lineups-%d.db",  year);
  sprintf(oldfilename, "db/lineups-%d.old", year);
  sprintf(newfilename, "db/lineups-%d.new", year);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    for (int j=0; j<DB_COLS; j++) fprintf(f, "%s,", ldb[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

void SaveEvents(int year) {
  char oldfilename[64];
  char newfilename[64];
  char filename[64];
  sprintf(filename,    "db/events-%d.db",  year);
  sprintf(oldfilename, "db/events-%d.old", year);
  sprintf(newfilename, "db/events-%d.new", year);
  FILE *f = fopen(newfilename, "wt");
  if (f==NULL) { fprintf(stderr, "ERROR: database %s not found.\n", newfilename); return; }
  for (int i=0; i<NT*(NT-1); i++) {
    for (int j=0; j<EV_COLS; j++) fprintf(f, "%s,", edb[i][j]);
    fprintf(f, "\n");
  }
  fclose(f);
  rename(filename, oldfilename);
  rename(newfilename, filename);
}

int LoadCatalog(int year) {
//burdca,Carol,Burdan,00/00/1912,ROM,173,#,A,3,3,0,-11
  char filename[64], s[5000], *tk[CAT_COLS];
  FILE *f;
  sprintf(filename, "catalogs/catalog-%d.dat", year);
  f = fopen(filename, "rt");
  if (f==NULL) { fprintf(stderr, "ERROR: catalog %s not found.\n", filename); return 0; }
  int i = 0;
  while (!feof(f)) {
    fgets(s, 5000, f);
    if (strlen(s)<20) continue;
    tk[0] = strtok(s, ",\n");
    for (int j=1; j<CAT_COLS; j++) tk[j]=strtok(NULL, ",\n");
        for (int j=0; j<CAT_COLS; ++j) {
            if (tk[j]) strcpy(cat[i][j], tk[j]); else strcpy(cat[i][j], " ");
        }
    s[0] = 0;
    i++;
  }
  fclose(f);
  ncat = i;
//  fprintf(stderr, "Loaded %d catalog entries from %s.\n", ncat, filename);
  return 1;
}

int FindInCatalog(char *s, int t) {
  Tokenize(s);
  for (int i=0; i<ncat; i++) {
    int ct = atoi(cat[i][5]);
    if (t==ct) {
      if (strstr(cat[i][1], fns)!=NULL && strcmp(cat[i][2], lns)==0) {
        return i;
      }
    }
  }
  return -1;
}

int FindCoach(char *s) {
  char t[1024];
  strncpy(t, s, 1024);
  Tokenize(t);
  return Co->FindName(fns, lns);
}

int FindReferee(char *s) {
  char t[1024];
  strncpy(t, s, 1024);
  Tokenize(t);
  return Rf->FindName(fns, lns);
}

void ExtractHEvent(int e, char *sp, char *sm) {
  char *name = strtok(sp, "(");
  rtrim(name);
  sanitize(name);
  char *spec = strtok(NULL, ")");
  char *min = strtok(sm, "&");
  evm[e] = atoi(min);
  if (spec!=NULL) {
    if (strstr(spec, "aut")!=NULL) {
      evt[e] = EV_OWNGOAL;
      evp[e] = FindInCatalog(sp, id[away]);
    } else if (strstr(spec, "rig")!=NULL) {
      evt[e] = EV_PKGOAL;
      evp[e] = FindInCatalog(sp, id[home]);
    }
  } else {
    evt[e] = EV_GOAL;
    evp[e] = FindInCatalog(sp, id[home]);
  }
}

void ExtractAEvent(int e, char *sp, char *sm) {
  char *name = strtok(sp, "(");
  rtrim(name);
  sanitize(name);
  char *spec = strtok(NULL, ")");
  char *min = strtok(sm, "&");
  evm[e] = atoi(min);
  if (spec!=NULL) {
    if (strstr(spec, "aut")!=NULL) {
      evt[e] = EV_OWNGOAL;
      evp[e] = FindInCatalog(sp, id[home]);
    } else if (strstr(spec, "rig")!=NULL) {
      evt[e] = EV_PKGOAL;
      evp[e] = FindInCatalog(sp, id[away]);
    }
  } else {
    evt[e] = EV_GOAL;
    evp[e] = FindInCatalog(sp, id[away]);
  }
}

int ExtractMin(char *s) {
  if (s==NULL) return -1;
  char *tap = strstr(s, "&#39;");
  if (tap) tap[0] = 0;
  char *tbsp = strstr(s, "&nbsp;");
  if (tbsp) tbsp += 6; else tbsp = s;
  tbsp = ltrim(tbsp);
  int m = atoi(tbsp);
  return (m>0 ? m : 0);
}

//---------------------

int main(int argc, char **args) {
  if (argc < 2) {
    fprintf(stderr, "ERROR: missing args.");
  }
  int season = atoi(args[1])+1;
  int r = atoi(args[2]);

  Load();
  LoadSeason(season);
  LoadDB(season);
  LoadEvents(season);
  LoadCatalog(season);
  Pl->Load("catalogs/players.dat");
  Rf->Load("catalogs/referees.dat");
  Co->Load("catalogs/coaches.dat");
  Loc->Load("catalogs/city.dat", "catalogs/stadium.dat");

  char filename[256];
  sprintf(filename, "tmp/%d/rep-%d.html", season-1, r);
  FILE *f = fopen(filename, "rt");
  if (!f) {
    fprintf(stderr, "ERROR: file %s not found.", filename);
  }

  for (int i=0; i<ROSTER_SIZE; i++) {
    hpid[i] = apid[i] = -1;
    hmin[i] = amin[i] = (i<11? 90 : 0);
  }

  char s[1024]; s[0] = 0;
  char *tk;
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "<table"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "<table"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "<table"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "<table"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "<table"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "<table"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "<table"))==NULL);

  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "GIORNATA"))==NULL);
  char *tround = strtok(tk + 9, " \n<");
  if (tround) round = atoi(tround);
  printf("----------------------------------------\n");
  printf(" > Round %d\n", round);

  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCell"))==NULL);
  fgets(s, 1024, f);
  char *tdate = strstr(s, "<b>");
  if (tdate) tdate = strtok(tdate + 3, "<");
  if (tdate) {
    char *twk = strtok(tdate, " ");
    char *tday = strtok(NULL, " ");
    char *tmo = strtok(NULL, " ");
    char *tyr = strtok(NULL, " ");
    if (tday) day = atoi(tday);
    if (tmo) month = findMonth(tmo);
    if (tyr) year = atoi(tyr);
  }
  printf(" > Date: %04d-%02d-%02d\n", year, month, day);

  char *tref = NULL;
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCell"))==NULL);
  fgets(s, 1024, f);
  if (strstr(s, "scheda_arbitro")!=NULL) {
    fgets(s, 1024, f);
    tref = ltrim(s);
    if (tref) {
      char *ter = strstr(tref, " di \n"); if (ter) ter[0] = 0;
      rtrim(tref);
    }
    printf(" > Referee: %s\n", tref);
    ref = FindReferee(tref);
  }

  char *tvenue = strstr(s, "<b>");
  char *tcity = NULL;;
  char *tstad = NULL;
  if (tvenue) tvenue = strtok(tvenue + 3, "<");
  if (tvenue) tcity = strtok(tvenue, "-");
  if (tcity) tcity[strlen(tcity)-1] = 0;
  rtrim(tcity);
  if (tcity) city = Loc->FindCityName(tcity);
  if (tcity) tstad = strtok(NULL, "(\n");
  if (tstad) tstad = ltrim(tstad);
  printf(" > City: %s\n", tcity);
  rtrim(tstad);
  if (tstad) stad = Loc->FindStadium(city, tstad);
  if (stad >= 0) printf(" > Stadium: %s [%s]\n", Loc->V[stad].name, Loc->V[stad].mnem);
  else printf("%s > Stadium %s - %s not found%s.\n", thiyellow, tcity, tstad, tnormal);

  if (!tref) {
    do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCell"))==NULL);
    do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "scheda_arbitro"))==NULL);
    fgets(s, 1024, f);
    char *tref = ltrim(s);
    if (tref) {
      char *ter = strstr(tref, " di \n"); if (ter) ter[0] = 0;
      rtrim(tref);
    }
    ref = FindReferee(tref);
    if (ref>=0) printf(" > Referee: %s %s [%s]\n", Rf->P[ref].pren, Rf->P[ref].name, Rf->P[ref].mnem);
    else printf("%s > Referee %s not found.%s\n", thiyellow, tref, tnormal);
    printf("----------------------------------------\n");
  }
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "<table"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "/societa/"))==NULL);
  fgets(s, 1024, f);
  char *thome = strtok(s, "\"");
  for (int i=1; i<6; i++) thome = strtok(NULL, "\"");
  home = FindNick(thome, season);
  if (home>=0) printf("%s ", mnem[id[home]]);
  else printf("%s > Team %s not found.%s\n", thiyellow, thome, tnormal);

  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "MainTitle"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "MainTitle"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "MainTitle"))==NULL);
  fgets(s, 1024, f);
  char *tx = ltrim(s);
  if (tx) hx = atoi(tx);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "MainTitle"))==NULL);
  fgets(s, 1024, f);
  char *ty = ltrim(s);
  if (ty) ax = atoi(ty);
  printf("%d-%d ", hx, ax);

  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "MainTitle"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "MainTitle"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "MainTitle"))==NULL);
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "/societa/"))==NULL);
  fgets(s, 1024, f);
  char *taway = strtok(s, "\"");
  for (int i=1; i<6; i++) taway = strtok(NULL, "\"");
  away = FindNick(taway, season);
  if (away>=0) printf(" %s\n", mnem[id[away]]);
  else printf("%s > Team %s not found.%s\n", thiyellow, taway, tnormal);
  printf("----------------------------------------\n");

  int rh = 0;
  int ra = 0;
  int rpk = 0;
  char *sap;
  char gsc[MAX_EVENTS][6][64];
  for (int i=0; i<MAX_EVENTS; i++) for (int j=0; j<6; j++) gsc[i][j][0] = 0;

  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "<tr>"))==NULL);

  int row = -1;
  int col = 0;
  int pkm = 0;
  do {
    fgets(s, 1024, f);
    if (strstr(s, "<td")!=NULL) { col++; }
    else if (strstr(s, "<tr>")!=NULL) { col=0; }
    else if (strstr(s, "</td>")!=NULL) {
      strtok(s, "<");
      char *tg = ltrim(s);
      if (col!=1 && col!=6) strtok(tg, "&");
      if (tg[0]==0 || strstr(tg, "&nbsp;")!=NULL) continue;
      strncpy(gsc[rh+ra][col-1], tg, 64);
      if (col==2) {
        printf(" > %2s'  (%d-%d)  %s\n", tg, rh+1, ra, gsc[rh+ra][0]);
        ExtractHEvent(rh+ra, gsc[rh+ra][0], gsc[rh+ra][1]);
        rh++;
      }
      else if (col==6) {
//        strtok(tg, "&");
        printf(" > %2s'  (%d-%d)  %s\n", gsc[rh+ra][4], rh, ra+1, tg);
        ExtractAEvent(rh+ra, gsc[rh+ra][5], gsc[rh+ra][4]);
        ra++;
      }
    }
    else if ((sap=strstr(s, "&#39;"))!=NULL) {
       strtok(s, "&");
       char *tpk = ltrim(s);
       pkm = atoi(tpk);
    }
    else if ((tk=strstr(s, "sbaglia"))!=NULL) {
      strtok(s, "(");
      char *trs = ltrim(s);
      rtrim(trs);
      char trs2[64]; strncpy(trs2, trs, 64);
	  int ph = FindInCatalog(trs, id[home]);
      int pa = FindInCatalog(trs2, id[away]);
      int en = rh+ra+rpk;
      evp[en] = (ph>=0 ? ph : pa);
      evm[en] = pkm;
      evt[en] = EV_PKMISS;
      rpk++;
    }
    else if ((tk=strstr(s, "para un calcio"))!=NULL) {
      char *trs = tk + 27;
      strtok(trs, "(");
      rtrim(trs);
      char trs2[64]; strncpy(trs2, trs, 64);
      int ph = FindInCatalog(trs, id[home]);
      int pa = FindInCatalog(trs2, id[away]);
      int en = rh+ra+rpk;
      evp[en] = (ph>=0 ? ph : pa);
      evm[en] = pkm;
      evt[en] = EV_PKSAVED;
      rpk++;
    }
  } while (!feof(f) && (tk=strstr(s, "TITOLARI"))==NULL);
  nev = rh+ra+rpk;

  printf("----------------------------------------------\n");
  for (int i=0; i<nev; i++) {
    printf("%6s%c%-3d,", cat[evp[i]][0], evsymb[evt[i]], evm[i]);
  }
  printf("\n");
  printf("----------------------------------------------\n");

  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "</tr>"))==NULL);

  row = -1;
  col = -1;
  int espm, subm, isesp, issub;
  char rost[22][10][64];
  int card[22][2];
  int subx[22][2];
  int suby[22][2];
  int numsubs = 0;
  for (int i=0; i<22; i++) {
    for (int j=0; j<10; j++) rost[i][j][0] = 0;
    card[i][0] = card[i][1] = 0;
    subx[i][0] = subx[i][1] = 0;
    suby[i][0] = suby[i][1] = 0;
  }
  do {
    fgets(s, 1024, f);
    if (strstr(s, "<tr>")!=NULL) {
     row++; col=-1;
    }
    else if (strstr(s, "<td")!=NULL) {
      col++;
      if (col==0 || col==9) {
        espm = 90; isesp = 0;
        do {
          fgets(s, 1024, f);
          if ((sap=strstr(s, "&#39;"))!=NULL) if (sap[-1]>='1' && sap[-1]<='9') espm = ExtractMin(s);
          if (strstr(s, "image/espulsione")!=NULL) isesp = 1;
        } while (strstr(s, "</td>")==NULL);
        if (isesp>0) card[row][col/5] = espm;
      }
      if (col==3 || col==6) {
        subm = 90; issub = 0;
        do {
          fgets(s, 1024, f);
          if ((sap=strstr(s, "&#39;"))!=NULL) if (sap[-1]>='1' && sap[-1]<='9') subm = ExtractMin(s);
          if (strstr(s, "image/uscito")!=NULL) issub = 1;
        } while (strstr(s, "</td>")==NULL);
        if (issub>0) subx[row][col/5] = subm;
        if (isesp>0) card[row][col/5] = espm;
      }
    }
    else if (strstr(s, "</td>")!=NULL) {
      strtok(s, "<");
      char *tg = ltrim(s);
      strncpy(rost[row][col], tg, 64);
    }
  } while (!feof(f) && (tk=strstr(s, "ALLENATORE"))==NULL && (tk=strstr(s, "DISPOSIZIONE"))==NULL);

  for (int i=0; i<11; i++) {
    printf("%-30s %s\n", rost[i][1], rost[i][8]);
  }

  do { fgets(s, 1024, f); } while (!feof(f) && (strstr(s, "</tr>"))==NULL);

  if (strstr(s, "DISPOSIZIONE")!=NULL) {
  row = 10;
  col = -1;
  do {
    fgets(s, 1024, f);
    if (strstr(s, "<tr>")!=NULL) {
     row++; col=-1;
    }
    else if (strstr(s, "<td")!=NULL) {
      col++;
      if (col==0 || col==9) {
        espm = 90; isesp = 0;
        do {
          fgets(s, 1024, f);
          if ((sap=strstr(s, "&#39;"))!=NULL) if (sap[-1]>='1' && sap[-1]<='9') espm = ExtractMin(s);
          if (strstr(s, "image/espulsione")!=NULL) isesp = 1;
        } while (strstr(s, "</td>")==NULL);
        if (isesp>0) card[row][col/5] = espm;
      }
      if (col==3 || col==6) {
        subm = 90; issub = 0;
        do {
          fgets(s, 1024, f);
          if ((sap=strstr(s, "&#39;"))!=NULL) if (sap[-1]>='1' && sap[-1]<='9') subm = ExtractMin(s);
          if (strstr(s, "image/entrato")!=NULL) issub = 1;
        } while (strstr(s, "</td>")==NULL);
        if (issub>0) suby[row][col/5] = subm;
        if (isesp>0) card[row][col/5] = espm;
      }
    }
    else if (strstr(s, "</td>")!=NULL) {
      strtok(s, "<");
      char *tg = ltrim(s);
      strncpy(rost[row][col], tg, 64);
    }
  } while (!feof(f) && (tk=strstr(s, "ALLENATORE"))==NULL);

  printf(" > Subs\n");
  for (int i=11; i<22; i++) {
    if (rost[i][1][0]!=0 || rost[i][8][0]!=0) {
      numsubs++;
      printf("%-30s %s\n", rost[i][1], rost[i][8]);
    }
  }
  } // subs

  col = -1;
  char coach[2][64];
  coach[0][0] = 0; coach[1][0] = 0;
  do {
    fgets(s, 1024, f);
    if (strstr(s, "scheda_allenatore")!=NULL) {
      fgets(s, 1024, f);
      strtok(s, "<");
      char *tco = ltrim(s);
      if (col < 1) strncpy(coach[++col], tco, 64);
    }
  } while (!feof(f) && (tk=strstr(s, "</table>"))==NULL);

  printf("----------------------------------------------\n");
  printf(" > Coaches:\n");
  printf("%-30s %s\n", coach[0], coach[1]);

  hco = FindCoach(coach[0]);
  aco = FindCoach(coach[1]);
  if (hco>=0) printf(" >   %s %s [%s]\n", Co->P[hco].pren, Co->P[hco].name, Co->P[hco].mnem);
  else printf("%s >  Coach %s not found %s\n", thiyellow, coach[0], tnormal);
  if (aco>=0) printf(" >   %s %s [%s]\n", Co->P[aco].pren, Co->P[aco].name, Co->P[aco].mnem);
  else printf("%s >  Coach %s not found %s\n", thiyellow, coach[1], tnormal);

  fclose(f);

  // -----------------------------------

  for (int i=0; i<11; i++) {
    hpid[i] = FindInCatalog(rost[i][1], id[home]);
    apid[i] = FindInCatalog(rost[i][8], id[away]);
    if (subx[i][0]>0) {
      hmin[i] = subx[i][0] -1;
    }
    if (card[i][0]>0) {
      hmin[i] = card[i][0] - 1;
      evp[nev] = hpid[i]; evm[nev] = card[i][0]; evt[nev] = EV_RED; nev++;
    }
    if (subx[i][1]>0) {
      amin[i] = subx[i][1] -1;
    }
    if (card[i][1]) {
      amin[i] = card[i][1] - 1;
      evp[nev] = apid[i]; evm[nev] = card[i][1]; evt[nev] = EV_RED; nev++;
    }
  }

  printf("----------------------------------------------\n");

  for (int i=0; i<11; i++) { printf("%3d ", hpid[i]); }; printf("\n");
  for (int i=0; i<11; i++) { printf("%3d ", apid[i]); }; printf("\n");

  printf("----------------------------------------------\n");

  // -----------------------------------

  if (numsubs > 0) {
    for (int i=11; i<22; i++) {
      hpid[i] = FindInCatalog(rost[i][1], id[home]);
      apid[i] = FindInCatalog(rost[i][8], id[away]);
      if (suby[i][0]>0) {
        if (card[i][0]>0) {
          hmin[i] = card[i][0] - suby[i][0];
          evp[nev] = hpid[i]; evm[nev] = card[i][0]; evt[nev] = EV_RED; nev++;
        } else {
          hmin[i] = 91 - suby[i][0];
        }
      }
      if (suby[i][1]>0) {
        if (card[i][1]>0) {
          amin[i] = card[i][1] - suby[i][1];
          evp[nev] = apid[i]; evm[nev] = card[i][1]; evt[nev] = EV_RED; nev++;
        } else {
          amin[i] = 91 - suby[i][1];
        }
      }
    }

    for (int i=11; i<22; i++) { printf("%3d ", hpid[i]); }; printf("\n");
    for (int i=11; i<22; i++) { printf("%3d ", apid[i]); }; printf("\n");
    printf("----------------------------------------------\n");
  }

  // -----------------------------------

  int mid = Mid(home, away);
  sprintf(ldb[mid][DB_DATE], "%04d-%02d-%02d@     ", year, month, day);
  sprintf(ldb[mid][DB_VENUE], "%s", tstad? Loc->V[stad].mnem : "      ");
  sprintf(ldb[mid][DB_ROUND], "%2d", round);
  if (strlen(coach[0])>0) sprintf(ldb[mid][DB_COACH1], "%s", Co->P[hco].mnem); else sprintf(ldb[mid][DB_COACH1], "      ");
  if (strlen(coach[1])>0) sprintf(ldb[mid][DB_COACH2], "%s", Co->P[aco].mnem); else sprintf(ldb[mid][DB_COACH2], "      ");
  sprintf(ldb[mid][DB_REF],    "%s", Rf->P[ref].mnem);
  for (int i=DB_ROSTER1; i<DB_ROSTER1+ROSTER_SIZE; i++) {
    int p = hpid[i-DB_ROSTER1];
    sprintf(ldb[mid][i], "%6s:%-3d", (p>=0? cat[p][0] : " "), hmin[i-DB_ROSTER1]);
  }
  for (int i=DB_ROSTER2; i<DB_ROSTER2+ROSTER_SIZE; i++) {
    int p = apid[i-DB_ROSTER2];
    sprintf(ldb[mid][i], "%6s:%-3d", (p>=0? cat[p][0] : " "), amin[i-DB_ROSTER2]);
  }

  for (int i=0; i<nev; i++) {
    sprintf(edb[mid][i], "%6s%c%-3d", cat[evp[i]][0], evsymb[evt[i]], evm[i]);
  }

  for (int i=0; i<DB_ROSTER1; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  for (int i=DB_ROSTER1; i<DB_ROSTER1+11; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  for (int i=DB_ROSTER1+11; i<DB_COACH1; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  printf("%s,\n", ldb[mid][DB_COACH1]);
  for (int i=DB_ROSTER2; i<DB_ROSTER2+11; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  for (int i=DB_ROSTER2+11; i<DB_COACH2; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  printf("%s,\n", ldb[mid][DB_COACH2]);
  for (int i=DB_REF; i<DB_COLS; i++) { printf("%s,", ldb[mid][i]); }  printf("\n");
  for (int i=0; i<EV_COLS; i++) { printf("%s,", edb[mid][i]); }  printf("\n");

  SaveDB(season);
  SaveEvents(season);

  return 0;
}
