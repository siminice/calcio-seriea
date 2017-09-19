#include "catalog.hh"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *normal  = "\033[0m";
const char *black   = "\033[30m";
const char *red     = "\033[31m";
const char *green   = "\033[32m";
const char *yellow  = "\033[33m";
const char *blue    = "\033[34m";
const char *magenta = "\033[35m";
const char *cyan    = "\033[36m";
const char *white   = "\033[37m";
const char *loblack   = "\033[30;0m";
const char *lored     = "\033[31;0m";
const char *logreen   = "\033[32;0m";
const char *loyellow  = "\033[33;0m";
const char *loblue    = "\033[34;0m";
const char *lomagenta = "\033[35;0m";
const char *locyan    = "\033[36;0m";
const char *lowhite   = "\033[37;0m";
const char *hiblack   = "\033[30;1m";
const char *hired     = "\033[31;1m";
const char *higreen   = "\033[32;1m";
const char *hiyellow  = "\033[33;1m";
const char *hiblue    = "\033[34;1m";
const char *himagenta = "\033[35;1m";
const char *hicyan    = "\033[36;1m";
const char *hiwhite   = "\033[37;1m";

const char* colmatch[] = {hiyellow, cyan, green};

int cMIN(int x, int y) {
  if (x<y) return x;
  else return y;
}

#define MAX_LEN 100
int d[MAX_LEN][MAX_LEN];

int cdist(char*a, char*b) {
  int i, j, modif, ins, del;
  int m = strlen(a);
  int n = strlen(b);
	if (m>=MAX_LEN || n>=MAX_LEN) return MAX_LEN;
  for (i=0; i<=m; i++) d[i][0] = i;
  for (i=0; i<=n; i++) d[0][i] = i;

  for (i=1; i<=m; i++)
    for (j=1; j<=n; j++) {
      if (a[i-1] == b[j-1]) modif = d[i-1][j-1];
        else modif = d[i-1][j-1] + 1;
      del = cMIN(d[i-1][j], d[i][j-1]) + 1;
      d[i][j] = cMIN(modif, del);
    }

  return d[m][n];
}

int GetYOB(char *s) {
	char w[32];
	if (!s) return -1;
	strncpy(w, s, 16);
	char *tkd = strtok(w, "/.-");
	char *tkm = strtok(NULL, "/.-");
	char *tky = strtok(NULL, "/.-");
	int y;
	if (tky == NULL) return 0;
    if (strlen(tky) == 4) {
		y = strtol(tky, NULL, 10);
		return y;
	}
	else {
		y = strtol(tkd, NULL, 10);
		return y;
	}
}

void copytk(char **pf, char *tk) {
	if (tk==NULL) {
		*pf = new char[2];
		strcpy(*pf, " ");
	}
	else {
		*pf = new char[strlen(tk)+1];
		strcpy(*pf, tk);
	}
}

char low(char c) {
  if (c=='ª') return 'º';
  if (c=='Þ') return 'þ';
  if (c=='Î') return 'î';
  if ((c>='A' && c<='Z')) return c+32;
	if (c=='Á' || c=='É' || c=='Ó' || c=='Í' || c=='Ú' || c=='Ö' || c=='Ü') return c+32;
  return c;
}

int Person::Fits(char *sn, char *sp, char *sd) {
	int fit = 100;
	if (strcmp(name, sn)!=0) {
		if (strstr(sn, name)==NULL) {
			fit -= 5*cdist(name, sn);
		}
		else fit -= 25;
	}
	if (sp[0]!=' ' && strcmp(pren, sp)!=0) {
		if (strstr(sp, pren)==NULL) {
			fit -= 2*cdist(pren, sp);
		}
		else fit -= 15;
	}
	int y1 = GetYOB(dob);
	int y2 = GetYOB(sd);
	int diff = y2-y1;
	if (y2<y1) diff = y1-y2;
	if (diff<10) diff = diff/2;
	if (diff>20) diff = 100;
	fit -= diff;
	return fit;
}

Stat::Stat() {
	sez = champ = promo = releg = 0;
	win = drw = los = gsc = gre = 0;
}

void Stat::reset() {
	sez = champ = promo = releg = 0;
	win = drw = los = gsc = gre = 0;
}

int Stat::numg() {
	return win+drw+los;
}

double Stat::pct() {
	int ng = win+drw+los;
	if (ng==0) return 0.0;
	return (win+0.5*drw)/ng;
}

void Stat::addRes(int x, int y) {
	gsc += x; gre += y;
	if (x>y) ++win;
	else if (x==y) ++drw;
	else ++los;
}

void Stat::add(Stat *x) {
	sez += x->sez;
	champ += x->champ;
	promo += x->promo;
	releg += x->releg;
	win	+= x->win;
	drw += x->drw;
	los	+= x->los;
	gsc += x->gsc;
	gre += x->gre;
}

int Stat::sup(Stat *x, int rule) {
	int ng1 = win+drw+los;
	int ng2 = x->win+x->drw+x->los;
	if (rule==RULE_NUMG) {
		if (ng1>ng2) return -1;
		else if (ng1==ng2) return 0;
		else return 1;
	}
	if (rule==RULE_PCT) {
		double pc1 = pct();
		double pc2 = x->pct();
		if (pc1>pc2) return -1;
		if (pc1<pc2) return 1;
		if (ng1>ng2) return -1;
		else return 1;
	}
	int pt1 = 2*win+drw;
	int pt2 = 2*x->win+x->drw;
	if (pt1>pt2) return -1;
	if (pt1<pt2) return 1;
	if (pt1==0) {
		if (ng1>0  && ng2==0) return -1;
		if (ng1==0 && ng2>0)  return 1;
	}
	int gd1 = gsc-gre;
	int gd2 = x->gsc-x->gre;
	if (gd1>gd2) return -1;
	if (gd1<gd2) return 1;
	if (gsc>x->gsc) return -1;
	return 1;
}

Ranking::Ranking(int an) {
	n = an;
	S = new Stat[n];
	rank = new int[n];
	for (int i=0; i<n; ++i) {
		S[i].reset();
		rank[i] = i;
	}
}

int Ranking::compare0(const void *c1, const void *c2) {
	int* i1 = (int*)c1;
	int* i2 = (int*)c2;
	return(S[*i1].sup(&S[*i2], 0));
}

int Ranking::compare1(const void *c1, const void *c2) {
	int* i1 = (int*)c1;
	int* i2 = (int*)c2;
	return(S[*i1].sup(&S[*i2], 1));
}

void Ranking::rqsort(int rule) {
/*
	int (*comp0)(const void*, const void*);
	int (*comp1)(const void*, const void*);
	comp0 = &compare0;
	comp1 = &compare1;
	if (rule==0)
		qsort(rank, n, sizeof(int), *comp0);
	else if (rule==1)
    qsort(rank, n, sizeof(int), *comp1);
*/
}

void Ranking::bubbleSort(int rule) {
	int sorted;
	do {
		sorted = 1;
		for (int i=0; i<n-1; ++i) {
			if (S[rank[i+1]].sup(&S[rank[i]], rule) < 0)	{
				int aux = rank[i]; rank[i] = rank[i+1]; rank[i+1] = aux;
				sorted = 0;
			}
		}
	} while (!sorted);
}

int Catalog::Load(const char *filename) {
  n = 0;
  int c;
  for (c=0; c<257; c++) borna[c] = -1;
  FILE *f = fopen(filename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Catalog %s not found.\n", filename);
    return -1;
  }

  char s[100], *t[10];
  fgets(s, 100, f);
  sscanf(s, "%d", &n);
  P = new Person[MAX_CATALOG];
  for (int i=0; i<n; i++) {
    fgets(s, 100, f);
    t[0] = strtok(s, ",\t\n");
    for (int j=1; j<10; j++) t[j] = strtok(NULL, ",\t\n");
    P[i].mnem = new char[7];
     strcpy(P[i].mnem, t[0]);
    P[i].name = new char[strlen(t[1])+1];
     strcpy(P[i].name, t[1]);
    P[i].pren = new char[strlen(t[2])+1];
     strcpy(P[i].pren, t[2]);

 		copytk(&(P[i].dob), t[3]);
 		copytk(&(P[i].cty), t[4]);
 		copytk(&(P[i].pob), t[5]);
 		copytk(&(P[i].jud), t[6]);

		if (i==0 || (i>0 && P[i-1].mnem[0]!=P[i].mnem[0])) {
			c = ((unsigned char) P[i].mnem[0]);
			borna[c] = i;
		}
	}
  fclose(f);

  c = ((unsigned char)P[n-1].mnem[0]);
  borna[c+1] = n;
  for (c=255; c>0; c--)
    if (borna[c]>=0 && borna[c-1]<0) borna[c-1] = borna[c];
	return 0;
}

int Catalog::FindMnem(char *s) {
  int c = ((unsigned char)s[0]);
  for (int i=borna[c]; i<borna[c+1]; i++)
    if (strcmp(P[i].mnem, s)==0) return i;
  return -1;
}

int Catalog::SearchMnem(char *s) {
  for (int i=0; i<n; i++)
    if (strcmp(P[i].mnem, s)==0) return i;
  return -1;
}

int Catalog::binFindMnem(char *s) {
  int c = ((unsigned char)s[0]);
	int lo = borna[c];
	int hi = borna[c+1]-1;
	int mid = (hi+lo)/2;
	while (lo<hi) {
		int comp = strcmp(P[mid].mnem, s);
    if (comp==0) return mid;
		if (comp<0) { hi = mid; } else { lo = mid+1; }
		mid = (lo+hi)/2;
	}
	if (strcmp(P[mid].mnem, s)==0) return mid;
  return -1;
}

int Catalog::FindNameUnique(char *sp, char *sn) {
	int exact = 0;
	int sub = 0;
	int loc = -1;
	for (int i=0; i<n; i++) {
		if (strcmp(P[i].name, sn)==0) {
			if (strcmp(P[i].pren, sp)==0) { exact++; loc = i; }
			else if (strstr(P[i].pren, sp)!=NULL) sub++;
		}
	}
	if (exact==1) return loc;
	return -1;
}

int Catalog::FindName(char *sp, char *sn) {
	int exact = 0;
	int sub = 0;
	int locx = -1;
	int locs = -1;
	for (int i=0; i<n; i++) {
		if (strcmp(P[i].name, sn)==0) {
			if (strcmp(P[i].pren, sp)==0) { exact++; locx = i; }
			else if (strstr(P[i].pren, sp)!=NULL) { sub++; locs = i; }
		}
	}
	if (exact==1) return locx;
	if (sub > 0) return locs;
    return -1;
}

int Catalog::ExactMatch(char *sp, char *sn, char *sy) {
  for (int i=0; i<n; i++) {
    if (strcmp(P[i].name, sn)==0) {
	  if (strstr(P[i].pren, sp)!=NULL) {
	    if (strstr(P[i].pren, sp)!=NULL) {
          int yob1 = GetYOB(P[i].dob);
          int yob2 = GetYOB(sy);
          if (yob1==yob2) return i;
        }
      }
    }
  }
  return -1;
}

int Catalog::Add(char *ins) {
  char s[5000], *tk[100];
  int ch, opt[1000], nopt, pa;
	char sn[64], sp[64], sd[64], sc[64];

	strcpy(s, ins);
	int len = strlen(s);
	for (int i=0; i<n; ++i) {
		if (s[i]=='_') s[i] = ' ';
	}
	tk[0] = strtok(s, ",");
	for (int j=1; j<7; ++j) tk[j] = strtok(NULL, ",");
	if (tk[0]) strcpy(sn, tk[0]); else strcpy(sn, "???");
	if (tk[1]) strcpy(sp, tk[1]); else strcpy(sp, " ");
	if (tk[2]) strcpy(sc, tk[2]); else strcpy(sc, "ROM");
	if (tk[3]) strcpy(sd, tk[3]); else strcpy(sd, "00/00/0000");

  nopt = 0;
	fprintf(stdout, "\n");
  for (int i=0; i<n; i++) {
    if (strcmp(P[i].name, sn)==0) {
      opt[nopt] = i;
      fprintf(stdout, "   [%2d] %s: %-12s, %-20s [%10s]\n", 
				nopt+2, P[i].mnem, P[i].name, P[i].pren, P[i].dob);
      nopt++;
    }
    else if (strstr(P[i].name, sn)!=NULL) {
      opt[nopt] = i;
      fprintf(stdout, "   [%2d] %s: %-12s, %-20s [%10s]\n", 
				nopt+2, P[i].mnem, P[i].name, P[i].pren, P[i].dob);
      nopt++;
    }
  }

      int add = 0;
			fprintf(stdout, "...%s           : %-12s, %-20s (%3s) [%10s] [0]=cancel [1]=add [2-%d]=use ",
				normal, sn, sp, sc, sd, nopt+2);
//			fprintf(stdout, "...[0]=cancel [1]=add [2-%d]=use ", nopt+2);
      scanf("%d", &add);
      if (add==1) {

        P[n].name = new char[strlen(sn)+1];
        strcpy(P[n].name, sn);
        P[n].pren = new char[strlen(sp)+1];
        strcpy(P[n].pren, sp);
        P[n].dob = new char[12];
        strcpy(P[n].dob, sd);
        P[n].cty = new char[4];
        strcpy(P[n].cty, sc);
        P[n].pob = new char[2];
        strcpy(P[n].pob, " ");
        P[n].jud = new char[2];
        strcpy(P[n].jud, " ");

        P[n].mnem = new char[7];
        P[n].mnem[0] = low((unsigned char)P[n].name[0]);
        int d = 0; int k=1;
        while (k<4) {
          if (k>=strlen(sn)) P[n].mnem[k]='x';
          else if (P[n].name[k+d]==' ' || P[n].name[k+d]==39) {d++; k--;}
          else P[n].mnem[k] = low((unsigned char)P[n].name[k+d]);
          k++;
        }
        if (sp!=NULL) {
          if (sp[0]==' ') P[n].mnem[4] = '_';
          else P[n].mnem[4] = low((unsigned char)sp[0]);
        }
        else P[n].mnem[4] = '_';
        if (sp!=NULL) {
          if (strlen(sp)<2) P[n].mnem[5] = '_';
          else if (sp[1]==' ') P[n].mnem[5] = '_';
          else P[n].mnem[5] = low((unsigned char)sp[1]);
        }
        P[n].mnem[6] = 0;
        if (SearchMnem(P[n].mnem)>=0) {
          fprintf(stderr, "...Mnemonic %s already in use.\n", P[n].mnem);
          int k=0;
          do {
            k++;
            P[n].mnem[5] = (char) (k+48);
          } while (SearchMnem(P[n].mnem)>=0);
        }
        fprintf(stdout, ".Added entry #%d, %s: %s %s [%s] (%s, %s %s)\n", n+1,
			P[n].mnem, P[n].pren, P[n].name, P[n].dob, P[n].cty, P[n].pob, P[n].jud);
        n++;

      }
      else if (add>=2 && add<=nopt+1) {
/*
        pa = opt[add-2];
        fprintf(stderr, " Use #%d %s: %s, %s.\n", pa, pmnem[pa], pname[pa], ppren[pa]);
*/
			}
	return 1;
}

int Catalog::AddSuggest(char *ins) {
  char s[1024], *tk[100], kol[64];
  int ch, opt[1000], nopt, pa;
	char sn[64], sp[64], sd[64], sc[64], sl[64], sb[64];
	int perfect_match;

	strcpy(s, ins);
	int len = strlen(s);
	for (int i=0; i<len; ++i) if (s[i]=='_') s[i] = ' ';
	tk[0] = strtok(s, ",\n");
	for (int j=1; j<7; ++j) tk[j] = strtok(NULL, ",");
	if (tk[0]) strcpy(sn, tk[0]); else strcpy(sn, "???");
	if (tk[1]) strcpy(sp, tk[1]); else strcpy(sp, " ");
	if (tk[2]) strcpy(sc, tk[2]); else strcpy(sc, "ROM");
	if (tk[3]) strcpy(sd, tk[3]); else strcpy(sd, "0000-00-00");
	if (tk[4]) strcpy(sl, tk[4]); else strcpy(sl, " ");
	if (tk[5]) strcpy(sb, tk[5]); else strcpy(sb, " ");

  fprintf(stdout, "\n");
  nopt = 0;
  perfect_match = 0;
  for (int i=0; i<n; i++) {
    if (strcmp(P[i].name, sn)==0) {
		if (strstr(P[i].pren, sp)!=NULL) {
			strcpy(kol, hiyellow);
		}
		else {
			strcpy(kol, hiwhite);
		}
      opt[nopt] = i;
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)]\n",
        kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud);
      nopt++;
	  int yob1 = GetYOB(P[i].dob);
	  int yob2 = GetYOB(sd);
	  if (strcmp(P[i].pren, sp)==0 && strcmp(P[i].cty, sc)==0 && yob2-yob1<=10)
		perfect_match = i+1;
    }
    else if (strstr(P[i].name, sn)!=NULL) {
			strcpy(kol, hired);
      opt[nopt] = i;
      fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)]\n",
        kol, nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud);
      nopt++;
    }
    else {
      int delta = cdist(P[i].name, sn);
      if (delta<=2) {
        opt[nopt] = i;
        fprintf(stdout, "   %s[%2d] %s: %-12s, %-20s%s (%3s) [%10s, @%s (%s)]\n",
  	      colmatch[delta], nopt+2, P[i].mnem, P[i].name, P[i].pren, normal, P[i].cty, P[i].dob, P[i].pob, P[i].jud);
				nopt++;
			}
		}
  }

    int add = 0;
	if (perfect_match) return perfect_match-1;
	fprintf(stdout, "...%s           : %-12s, %-20s (%3s) [%10s, @%s (%s)]   [0]=cancel [1]=add [2-%d]=use ",
		normal, sn, sp, sc, sd, sl, sb, nopt+2);
    scanf("%d", &add);
      if (add>1 && add<=nopt+2) {
		int k = opt[add-2];
        P[k].name = new char[strlen(sn)+1];
        strcpy(P[k].name, sn);
        P[k].pren = new char[strlen(sp)+1];
        strcpy(P[k].pren, sp);
        P[k].dob = new char[12];
        strcpy(P[k].dob, sd);
        P[k].cty = new char[4];
        strcpy(P[k].cty, sc);
        P[k].pob = new char[strlen(sl)+1];
        strcpy(P[k].pob, sl);
        P[k].jud = new char[4];
        strcpy(P[k].jud, sb);
	  }
      else if (add==1) {
        P[n].name = new char[strlen(sn)+1];
        strcpy(P[n].name, sn);
        P[n].pren = new char[strlen(sp)+1];
        strcpy(P[n].pren, sp);
        P[n].dob = new char[12];
        strcpy(P[n].dob, sd);
        P[n].cty = new char[4];
        strcpy(P[n].cty, sc);
        P[n].pob = new char[strlen(sl)+1];
        strcpy(P[n].pob, sl);
        P[n].jud = new char[4];
        strcpy(P[n].jud, sb);

        P[n].mnem = new char[7];
        P[n].mnem[0] = low((unsigned char)P[n].name[0]);
        int d = 0; int k=1;
        while (k<4) {
          if (k>=strlen(sn)) P[n].mnem[k]='x';
          else if (P[n].name[k+d]==' ') {d++; k--;}
          else P[n].mnem[k] = P[n].name[k+d];
          k++;
        }
        if (sp!=NULL) {
          if (sp[0]==' ') P[n].mnem[4] = '_';
          else P[n].mnem[4] = low((unsigned char)sp[0]);
        }
        else P[n].mnem[4] = '_';
        if (sp!=NULL) {
          if (strlen(sp)<2) P[n].mnem[5] = '_';
          else if (sp[1]==' ') P[n].mnem[5] = '_';
          else P[n].mnem[5] = low((unsigned char)sp[1]);
        }
        P[n].mnem[6] = 0;
        if (SearchMnem(P[n].mnem)>=0) {
          fprintf(stderr, "...Mnemonic %s already in use.\n", P[n].mnem);
          int k=0;
          do {
            k++;
            P[n].mnem[5] = (char) (k+48);
          } while (SearchMnem(P[n].mnem)>=0);
        }
        fprintf(stdout, ".Added entry #%d, %s: %s %s [%s, @%s (%s)] (%s)\n", n+1,
			P[n].mnem, P[n].pren, P[n].name, P[n].dob, P[n].pob, P[n].jud, P[n].cty);
        n++;

      }
	return n;
}

void Catalog::ForceAdd(Person p) {
  P[n].mnem = strdup(p.mnem);
  P[n].name = strdup(p.name);
  P[n].pren = strdup(p.pren);
  P[n].dob = strdup(p.dob);
  P[n].cty = strdup(p.cty);
  P[n].pob = strdup(p.pob);
  P[n].jud = strdup(p.jud);
  n++;
}

int Catalog::GetByLastname(char *s) {
  int opt[1000];
  int nopt = 0;
  int ch;
  for (int i=0; i<n; i++) {
    if (strcmp(s, P[i].name)==0) {
       opt[nopt++] = i;
    }
  }
  if (nopt==0) return -1;
  if (nopt==1) return opt[0];
  do {
    for (int i=0; i<nopt; i++) {
      fprintf(stderr, "\n... [%d]. %s %s \t[%10s] ", i+1,
       P[opt[i]].pren, P[opt[i]].name, P[opt[i]].dob);
    }
    scanf("%d", &ch);
  } while (ch<=0 || ch>nopt);
  return opt[ch-1];
}

void Catalog::StartsWith(char *patt, int *opt, int &n) {
	n = 0;
	if (!patt) return;
	char cappatt[128];
	strncpy(cappatt, patt, 128);
	unsigned char sc = (unsigned char) cappatt[0];
	if (sc>='A' && sc<='Z') sc += 32;
	if (cappatt[0]>='a' && cappatt[0]<='z') cappatt[0] -= 32;
	if (cappatt[0]=='º') cappatt[0] = 'ª';
	if (cappatt[0]=='þ') cappatt[0] = 'Þ';
	for (int i=borna[sc]; i<borna[sc+1]; i++) {
		if (strstr(P[i].name, cappatt)==P[i].name ||
				strstr(P[i].mnem, patt)==P[i].mnem) {
			opt[n++] = i;
		}
	}
}

int Catalog::BestMatch(char *sn, char *sp, int delta) {
  int opt[1000];
  int nopt = 0;
  int ch;
  for (int i=0; i<n; i++) {
    if (strcmp(sn, P[i].name)==0) {
       opt[nopt++] = i;
    }
  }
  if (nopt==1) return opt[0];

  for (int i=0; i<n; i++) {
    int d = cdist(P[i].name, sn);
    if (d>0 && d<=delta) {
       opt[nopt++] = i;
    }
  }

  if (nopt<1) return -1;
  do {
    for (int i=0; i<nopt; i++) {
      fprintf(stderr, "\n... [%d]. %s %s \t[%10s] ", i+1,
       P[opt[i]].pren, P[opt[i]].name, P[opt[i]].dob);
    }
    scanf("%d", &ch);
  } while (ch<=0 || ch>nopt);
  return opt[ch-1];
}

void Catalog::GetInitial(int i, char *pini) {
    pini[1] = '.'; pini[2] = 0;
    if (i<0 || P[i].pren==NULL || P[i].pren[0]==' ') pini[0] = 0;
    else if (strlen(P[i].pren)>0) pini[0] = P[i].pren[0];
    else pini[0] = 0;
}


int Catalog::Save(const char *filename) {
  char nfilename[128];
  char ofilename[128];
	sprintf(nfilename, "%s.new", filename);
	sprintf(ofilename, "%s.old", filename);
  FILE *f = fopen(nfilename, "wt");
	if (f==NULL) {
		fprintf(stderr, "ERROR: cannot open file %s.\n", nfilename);
		return -1;
	}
  fprintf(f, "%d\n", n);
  for (int i=0; i<n; i++) {
    fprintf(f, "%s,%s,%s,%s,%s,%s,%s\n", P[i].mnem, P[i].name, P[i].pren, P[i].dob, P[i].cty, P[i].pob, P[i].jud);
  }
  fclose(f);
  rename(filename, ofilename);
  rename(nfilename, filename);
	return 0;
}

/* ******************************************************* */

Alias::Alias(char *an, int afy, int aly) {
	int len = strlen(an);
	name = new char[len+1];
	strcpy(name, an);
	fy = afy;
	ly = aly;
	next = NULL;
}

char * Venue::getName(int year) {
	Alias *a = alias;
	while (a!=NULL) {
		if (a->fy<=year && year<=a->ly) return a->name;
		a = a->next;
	}
	return name;
}

int Locations::Load(const char *cfilename, const char *vfilename) {
  char s[1024], *tk[100];
  nc = 0;
  FILE *f = fopen(cfilename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: file %s not found.\n", cfilename);
    return -1;
  }
  fgets(s, 100, f);
  sscanf(s, "%d", &nc);
  C = new City[nc];
  for (int i=0; i<nc; i++) {
    fgets(s, 100, f);
    tk[0] = strtok(s, ",\t\n");
    for (int j=1; j<4; j++) tk[j] = strtok(NULL, ",\t\n");
 		copytk(&(C[i].mnem), tk[0]);
 		copytk(&(C[i].name), tk[1]);
 		copytk(&(C[i].jud), tk[2]);
	}
  fclose(f);

	char sc[5];
  nv = 0;
  f = fopen(vfilename, "rt");
  if (f==NULL) {
    fprintf(stderr, "ERROR: file %s not found.\n", vfilename);
    return -1;
  }
  fgets(s, 10, f);
  sscanf(s, "%d", &nv);
	int y;
	Alias *a, *b;
  V = new Venue[nv];
  for (int i=0; i<nv; i++) {
    fgets(s, 1000, f);
    tk[0] = strtok(s, ",\t\n");
    for (int j=1; j<16; j++) tk[j] = strtok(NULL, ",\t\n");
 		copytk(&(V[i].mnem), tk[0]);
 		copytk(&(V[i].name), tk[1]);
    strncpy(sc, V[i].mnem, 4); sc[4] = 0;
		int ci = FindCity(sc);
	  V[i].city = ci;
 		if (tk[2]) V[i].capacity = atoi(tk[2]);
		if (tk[3]) V[i].built = atoi(tk[3]);
		a = new Alias(tk[1], V[i].built, 9999);
		V[i].alias = a;
		for (int j=4; tk[j]!=NULL; j+=2) {
			if (tk[j+1]) y = atoi(tk[j+1]); else y = 0;
			b = new Alias(tk[j], 0, y);
			a->fy = y+1;
			b->next = a;
			V[i].alias = b;
			a = b;
		}
/*
		if (ci>=0) fprintf(stderr, "Located venue %s in %s.\n", V[i].name, C[ci].name);
		else fprintf(stderr, "Cannot locate venue %s.\n", V[i].mnem);
*/
	}
  fclose(f);

	return 0;
}

void Locations::StartsWith(char *patt, int *opt, int &n) {
	n = 0;
	if (!patt) return;
	for (int i=0; i<nv; i++) {
		if (strstr(V[i].mnem, patt)==V[i].mnem) opt[n++] = i;
	}
}

int Locations::FindCity(char *s) {
  for (int i=0; i<nc; i++) {
    if (strcmp(C[i].mnem, s)==0) return i;
  }
  return -1;
}

int Locations::FindCityName(char *s) {
  for (int i=0; i<nc; i++) {
    if (strcmp(C[i].name, s)==0) return i;
  }
  return -1;
}

int Locations::FindVenue(char *s) {
  for (int i=0; i<nv; i++) {
    if (strcmp(V[i].mnem, s)==0) return i;
  }
  return -1;
}

int Locations::FindStadium(int city, char *s) {
  for (int i=0; i<nv; i++) {
    if (strcmp(V[i].name, s)==0) {
      if (city<0) {
        return i;
      }
      if (strstr(V[i].mnem, C[city].mnem)==V[i].mnem) {
        return i;
      }
    }
  }
  return -1;
}
