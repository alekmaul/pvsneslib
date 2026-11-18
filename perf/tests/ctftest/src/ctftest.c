/* ctftest.c - constify : les tableaux const a 2 dimensions, que tcc place en
   .data, doivent passer en .rodata (ROM) quelle que soit l'ecriture de la
   declaration ; une variable ordinaire declaree avant reste en RAM et
   modifiable. constify reconnaissait mal l accolade ouvrante en fin de ligne (le tableau
   restait en RAM) et un espace avant le crochet (il deplacait alors la premiere
   variable du fichier en ROM). Les accolades et les mots "const" des
   commentaires, des chaines et des caracteres ne comptent pas : { ici. */
#include <snes.h>

typedef union { const void *p; u16 w[2]; } ptr_u;

u16 score[2] = {10, 20};
u16 counter[2] = {30, 40};   // pas const : reste en RAM [x] = {
const char *brace_str = "a\"{";   /* chaine avec accolade */
u8 brace_chr = '{';
/* commentaire sur plusieurs lignes {
   const u16 fake[][2] = {{0, 0}};
*/

const u16 t_nextline[][2]=
{
	{1, 2}, {3, 4}
};
const u16 t_brace_eol[][2]={
	{5, 6}, {7, 8}
};
const u16 t_space_brace[][2] = {
	{9, 10}, {11, 12}
};
const u16 t_oneline[][2]={{13, 14}, {15, 16}};
const u16 t_space_bracket [][2] = {{17, 18}, {19, 20}};
const u16	t_tab[2][2]={
{21, 22}, {23, 24}};

static u16 err, n;

static void check(u16 got, u16 want)
{
    if (got != want) err++;
    n++;
}

static u16 bank(const void *p)
{
    ptr_u u;
    u.p = p;
    return u.w[1];
}

int main(void)
{
    err = 0; n = 0;
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    /* valeurs */
    check(t_nextline[1][1], 4);
    check(t_brace_eol[1][0], 7);
    check(t_space_brace[0][1], 10);
    check(t_oneline[1][1], 16);
    check(t_space_bracket[1][0], 19);
    check(t_tab[1][1], 24);

    /* constantes en ROM (pas en WRAM $7E/$7F) */
    check(bank(t_nextline) < 0x7E, 1);
    check(bank(t_brace_eol) < 0x7E, 1);
    check(bank(t_space_brace) < 0x7E, 1);
    check(bank(t_oneline) < 0x7E, 1);
    check(bank(t_space_bracket) < 0x7E, 1);
    check(bank(t_tab) < 0x7E, 1);

    /* variables ordinaires : en RAM, initialisees et modifiables */
    check(bank(counter) >= 0x7E, 1);
    check(counter[1], 40);
    counter[1]++;
    check(counter[1], 41);
    check(brace_chr, '{');
    check(brace_str[2], '{');
    check(bank(score) >= 0x7E, 1);   /* WRAM, banque $7E ou $7F */
    check(score[0], 10);
    check(score[1], 20);
    score[0] += 5;
    check(score[0], 15);

    consoleDrawText(1, 1, "CTF TESTS %u", n);
    consoleDrawText(1, 2, "CTF ERR %u", err);
    consoleDrawText(1, 4, (err || n != 21) ? "ECHEC" : "OK");
    while (1) WaitForVBlank();
    return 0;
}
