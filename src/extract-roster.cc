#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "catalog.hh"

Catalog *Pl = new Catalog();
char *fn = new char[256];
char *ln = new char[256];
char roster[100][4][64];
int nr = 0;

char* ltrim(char *s) {
  if (!s) return NULL;
  int i = 0;
  while (s[i]==' ' || s[i]=='\t') i++;
  return s+i;
}

void rtrim(char *s) {
  if (!s) return;
  int i = strlen(s) - 1;
  while (i>0 && (s[i]==' ' || s[i]=='\t')) i--;
  s[i+1] = 0;
}

int isUpper(char *s) {
  if (!s) return 0;
  int l = strlen(s);
  for (int i=0; i<l; i++) {
    if (s[i]>='a' && s[i]<='z') return 0;
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
  fn[0] = 0; ln[0] = 0;
  for (int j = 1; j<6; j++) tk[j] = strtok(NULL, " ");
  int ntf = 0;
  int ntl = 0;
  for (int j=0; j<6; j++) {
    if (tk[j]==NULL) continue;
    if (isUpper(tk[j])) {
      if (ntl>0) { strcat(ln, " "); }
      toCamel(tk[j]);
      strcat(ln, tk[j]);
      ntl++;
    } else {
      if (ntf>0) { strcat(fn, " "); }
      strcat(fn, tk[j]);
      ntf++;
    }
  }
  strncpy(roster[nr][0], fn, 64);
  strncpy(roster[nr][1], ln, 64);
}

int main(int argc, char **args) {
  if (argc < 3) {
    fprintf(stderr, "ERROR: missing args.");
  }
  Pl->Load("catalogs/players.dat");
  int season = atoi(args[1]);
  int t = atoi(args[2]);
  int tid = atoi(args[3]);
  char filename[256];
  sprintf(filename, "tmp/%d/t%d.html", season, t);
  FILE *f = fopen(filename, "rt");
  if (!f) {
    fprintf(stderr, "ERROR: file %s not found.", filename);
  }
  char s[1024]; s[0] = 0;
  char *tk;
  do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "PORTIERI"))==NULL);

  do {
    fgets(s, 1024, f);
    if (strstr(s, "scheda_giocatore")!=NULL) {
      fgets(s, 1024, f);
      char *tp = strtok(s, "<");
      tp = ltrim(tp);
      fprintf(stderr, "P %-30s", tp);
      Tokenize(tp);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      char *td = strtok(s, ">");
      char *tdt = strtok(NULL, "<");
      fprintf(stderr," %s \n", tdt);
      strncpy(roster[nr][2], tdt, 64);
      strcpy(roster[nr++][3], "P");
    }
  } while (!feof(f) && (tk=strstr(s, "DIFENSORI"))==NULL);

  do {
    fgets(s, 1024, f);
    if (strstr(s, "scheda_giocatore")!=NULL) {
      fgets(s, 1024, f);
      char *tp = strtok(s, "<");
      tp = ltrim(tp);
      fprintf(stderr, "F %-30s", tp);
      Tokenize(tp);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      char *td = strtok(s, ">");
      char *tdt = strtok(NULL, "<");
      fprintf(stderr, " %s \n", tdt);
      strncpy(roster[nr][2], tdt, 64);
      strcpy(roster[nr++][3], "F");
    }
  } while (!feof(f) && (tk=strstr(s, "CENTROCAMPISTI"))==NULL);

  do {
    fgets(s, 1024, f);
    if (strstr(s, "scheda_giocatore")!=NULL) {
      fgets(s, 1024, f);
      char *tp = strtok(s, "<");
      tp = ltrim(tp);
      fprintf(stderr, "M %-30s", tp);
      Tokenize(tp);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      char *td = strtok(s, ">");
      char *tdt = strtok(NULL, "<");
      fprintf(stderr, " %s \n", tdt);
      strncpy(roster[nr][2], tdt, 64);
      strcpy(roster[nr++][3], "M");
    }
  } while (!feof(f) && (tk=strstr(s, "ATTACCANTI"))==NULL);

  do {
    fgets(s, 1024, f);
    if (strstr(s, "scheda_giocatore")!=NULL) {
      fgets(s, 1024, f);
      char *tp = strtok(s, "<");
      tp = ltrim(tp);
      fprintf(stderr, "A %-30s", tp);
      Tokenize(tp);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      do { fgets(s, 1024, f); } while (!feof(f) && (tk=strstr(s, "TableCellBorder"))==NULL);
      char *td = strtok(s, ">");
      char *tdt = strtok(NULL, "<");
      fprintf(stderr, " %s \n", tdt);
      strncpy(roster[nr][2], tdt, 64);
      strcpy(roster[nr++][3], "A");
    }
  } while (!feof(f) && (tk=strstr(s, "</table>"))==NULL);

  fclose(f);

  int nxr = 0;
  for (int i=0; i<nr; i++) {
    int p = Pl->ExactMatch(roster[i][0], roster[i][1], roster[i][2]);
    if (p>=0) {
      //chelma,Marius,Chelaru,1997-03-02,ROM,291,97,M,0,0,0,0
      printf("%s,%s,%s,%s,%s,%d,#,%s,0,0,0,0\n", Pl->P[p].mnem, Pl->P[p].pren, Pl->P[p].name,
              Pl->P[p].dob, Pl->P[p].cty, tid, roster[i][3]);
      nxr++;
    } else {
      fprintf(stderr, "%s %s %s %s not found.\n", roster[i][0], roster[i][1], roster[i][2], roster[i][3]);
   }
  }
  fprintf(stderr, "%d/%d\n", nxr, nr);

  return 0;
}
