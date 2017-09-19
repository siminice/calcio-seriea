#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "catalog.hh"

#define NUM_PARTICLES 28

const char *particle[] = {
	"Van", "van", "Von", "von", "Al",    "El", "Della", "Dal", "Dalla", "Da",
    "De", "Dalle", "Di",  "Do",  "da",   "de",   "di",  "do",    "du", "Dos", 
    "Mc", "La",    "Le",  "le",  "Lo",   "Ben", "Del", "del"};

char ctty[64];
int  year;

void tolower(char *s) {
  if (!s) return;
  int len = strlen(s);
  if (s[0]==34) {
    for (int i=0; i<len; ++i) s[i] = s[i+1];
    len--;
  }
  for (int i=1; i<len; ++i) {
    if (s[i]>='A' && s[i]<='Z') {
			if (i>0 && s[i-1]!='-' && s[i-1]!='.') s[i] += 32;
		}
    else if (s[i]=='Á' || s[i]=='É' || s[i]=='Í' || s[i]=='Õ' || s[i]=='Ú') s[i]+=32;
  }
  if (s[len-1]=='"') { s[len-1] = 0; len--; }
}

int is_capitalized(char *s) {
  if (!s) return 0;
  int len = strlen(s);
  int num_cap = 0;
  for (int i=1; i<len; ++i) {
    if (s[i]>='A' && s[i]<='Z') num_cap++;
  }
  if (num_cap>len/2) return 1;
  else return 0;
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

void Extract(char *s, char *sp, char *sn) {
	int is_cap[10], num_cap, n;
	char tok[512], *tkpi[10];
	int partic;

	strncpy(tok, s, 512);
    sanitize(tok);
    tkpi[0] = strtok(tok, " ");
//		if (tkpi[0]) {
//			while ((tkpi[0][0]>='0' && tkpi[0][0]<='9') || tkpi[0][0]=='.') tkpi[0] = tkpi[0]+1;
//		}
    for (int j=1; j<10; ++j) tkpi[j] = strtok(NULL, " ");
		num_cap = 0;
        n = 0;
		while (tkpi[n]) {
			if (is_capitalized(tkpi[n])) {
				is_cap[n] = 1;
				num_cap++;
			}
			else is_cap[n] = 0;
			tolower(tkpi[n]);
			n++;
		}

    if (n>0) strncpy(sn, tkpi[n-1], 64);
		strcpy(sp, " ");
		if (num_cap>0) {
			sn[0] = 0;
			sp[0] = 0;
			int nl = 0;
			int np = 0;
			for (int i=0; i<n; ++i) {
				if (is_cap[i]) {
					if (nl>0) strcat(sn, " ");
					strcat(sn, tkpi[i]);
					nl++;
				}
				else {
					if (np>0) strcat(sp, " ");
					strcat(sp, tkpi[i]);
					np++;
				}
			}
		}
		else {
			partic = 0;
			if (n>1) strcpy(sp, tkpi[0]);
			for (int j=1; j<n-2; ++j) {
				strcat(sp, " ");
				strcat(sp, tkpi[j]);
			}
			if (n>=2) {
				for (int i=0; i<NUM_PARTICLES; ++i) {
					if (strcmp(tkpi[n-2], particle[i])==0) {
						sprintf(sn, "%s %s", tkpi[n-2], tkpi[n-1]);
						partic = 1;
					}
				}
				if (partic==0 && n>2) {
					strcat(sp, " ");
					strcat(sp, tkpi[n-2]);
				}
			}
		}

}

void AddOne(char *w, Catalog *c) {
//dãncio,Dãnciulescu,Ionel Daniel,06.12.76,ROM,Slatina,
  char *tok[12];
	char spren[64], slnam[64], ins[256];
	char blank[3];
	char dob[20];
    char pob[64];
    char cob[4];
	sprintf(blank, " ");
	sprintf(dob, "00/00/0000");
    sprintf(pob, " ");
    sprintf(cob, " ");
	if (!w) return;
    sanitize(w);
	tok[0] = strtok(w, ",\n");
	for (int j=1; j<10; ++j) tok[j] = strtok(NULL, ",\n");
	if (!tok[0]) return;
	Extract(tok[0], spren, slnam);
	if (!tok[1]) tok[1] = blank;
	if (!tok[2]) tok[2] = dob;
	if (!tok[3]) tok[3] = pob;
	if (!tok[4]) tok[4] = cob;
	sprintf(ins, "%s,%s,%s,%s,%s,%s", slnam, spren, tok[1], tok[2], tok[3], tok[4]);
	c->AddSuggest(ins);
}

void AddLineup(char *w, Catalog *c) {
  char *tks, *tkt, *tkl, *tkc, *tkm, *tkp, *tkpi[10], *tkn, *tkf;
  char *tok[30], s[1024], spren[256], slnam[256], ins[512];
  int nr, ntr, npr, pos, min;
	strncpy(s, w, 1024);
  tkt = strtok(s, ":");
  tkl = strtok(NULL, "\n");
	if (!tkl) tkl = tkt;
  tkc = strrchr(tkl, '.');
	if (tkc) tkc[0] = 0;
  tok[0] = strtok(tkl, ",;\n");
  for (int i=1; i<22; ++i) tok[i] = strtok(NULL, ",;\n");
  nr = 0; while (tok[nr]) nr++;
  ntr = nr;
  for (int i=0; i<nr; ++i) {
    char *spar = strchr(tok[i], '(');
    tks = NULL;
    if (spar) {
      strtok(tok[i], "(");
      tks = strtok(spar+1, ")");
    }
		Extract(tok[i], spren, slnam);
		sprintf(ins, "%s,%s,%s,00/00/%d", slnam, spren, ctty, year);
		printf("\n#%d", i+1);
		c->AddSuggest(ins);

    if (tks) {
			if (strlen(tks)>5) {
	      tkm = strtok(tks, " .");
				tkp = strtok(NULL, ")");
				Extract(tkp, spren, slnam);
				sprintf(ins, "%s,%s,%s,00/00/%d", slnam, spren, ctty, year);
				c->AddSuggest(ins);
				ntr++;
			}
    }

  }
}

void AddFromFile(char *filename, Catalog *cat) {
  FILE *f = fopen(filename, "rt");
  char s[512];
  if (!f) return;
  while (!feof(f)) {
    fgets(s, 512, f);
    if (feof(f) || s[0]==0) continue;
    s[strlen(s)-1] = 0;
    AddOne(s, cat);
    s[0] = 0;
  }
  fclose(f);
}

int main(int argc, char **argv) {
	int opt;
	char sline[1024];
	char filename[128];
	char scriptfile[128]; scriptfile[0] = 0;
	strcpy(filename, "/home/radu/lineups/catalogs/players.dat");
	if (argc>1) { strcpy(filename, argv[1]); }
	if (argc>2) { strcpy(scriptfile, argv[2]); }

	Catalog *cat = new Catalog();
	cat->Load(filename);
    printf("Loaded %d entries.\n", cat->Size());
	strcpy(ctty, "ROM");
	year = 1956;
    if (scriptfile[0]!=0) AddFromFile(scriptfile, cat);
	do {
		printf("[1] Input line [2] Save [3] Year (%d) [4] Country (%s) [5] Add one [6] From file [0] Quit\n  ", year, ctty);
		scanf("%d", &opt);
		getc(stdin);
		switch(opt) {
			case 1:
				printf(">> Name: ");
				fgets(sline, 1024, stdin);
				AddLineup(sline, cat);
				break;
			case 2:
				cat->Save(filename);
				break;
			case 3:
				do {
					printf("Year: "); scanf("%d", &year);
				} while (year<1900 || year > 3000);
				break;
			case 4:
				do {
					printf("Country: "); scanf("%s", ctty);
				} while (strlen(ctty)!=3);
				break;
			case 5:
				printf("Info [Prenume Nume,Nat,DOB,pob,cob]: \n  >>");
				fgets(sline, 1024, stdin);
				AddOne(sline, cat);
				break;
			case 6:
				printf("Filename: ");
				fgets(sline, 1024, stdin);
                sline[strlen(sline)-1] = 0;
				AddFromFile(sline, cat);
				break;
		}
	} while (opt);
	cat->Save(filename);
}
