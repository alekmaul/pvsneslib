
;
;       version : 10/20/97
;
;
;       some helpful system routine addresses
;
;
;       For now refer to existing TI-85 or TI-83 documentation
;       from the WEB.
;
;
.define _clrLCD                                       $4A7E
.define _clrScrn                                      $4A82
.define _putmap                                       $4A27
.define _putc                                         $4A2B
.define _puts                                         $4A37
.define _putps                                        $4A3B
.define _vputmap                                      $4AA1
.define _vputs                                        $4AA5
.define _vputsn                                       $4AA9
.define _GRBUFCPY                                     $4E39
.define _ILine                                        $4E51
.define _IPoint                                       $4E59
.define _PDspGrph                                     $4D6F
.define _CLine                                        $51E9
.define _getkey                                       $55AA
.define _getky                                        $5371
.define _RANDOM                                       $55DA
.define _newline                                      $4A5F
.define _runindicoff                                  $4AB1
.define _runindicon                                   $4AAD
.define _CONVOP1                                      $5577
;
;       VARIABLE ROUTINES SEE FILE 'VARS.DOC' & '
;
.define _RCLY                 $4CDF   ; rcl y to OP1
.define _RCLX                 $4CE3   ; rcl x to OP1
;
.define _CREATEREAL           $46EB   ; CREATE REAL VAR OP1, NO INPUTS
;
.define _CREATECPLX           $46F7   ; CREATE CPLX VAR OP1, NO INPUTS
;
.define _CREATERCONST         $46EF   ; CREATE REAL CONSTANT VAR OP1, NO INPUTS
.define _CREATECCONST         $46F3   ; CREATE CPLX CONSTANT VAR OP1, NO INPUTS
;
.define _CREATERVECT          $46FF   ; CREATE REAL VECTOR OP1, L = # ELEMENTS
.define _CREATECVECT          $4707   ; CREATE CPLX VECTOR OP1, L = # ELEMENTS
;
.define _CREATERLIST          $470F   ; CREATE REAL LIST OP1, HL = # ELEMENTS
.define _CREATECLIST          $4717   ; CREATE CPLX LIST OP1, HL = # ELEMENTS
;
.define _CREATERMAT           $471F   ; CREATE REAL MATRIX OP1, HL = ROW,COL
.define _CREATECMAT           $4727   ; CREATE CPLX MATRIX OP1, HL = ROW,COL
;
.define _CREATESTRNG          $472F   ; CREATE STRING OP1, HL = # BYTES
;
.define _CREATE               $4737   ; CREATE  ATION OP1, HL = # BYTES
;
.define _CREATEPICT           $473B   ; CREATE PICTURE VAR OP1, NO INPUTS
;
.define _CREATEPROG           $474F   ; CREATE PROGRAM VAR OP1, HL = # BYTES
;
.define _FINDSYM              $46CB   ; LOOK UP VAR
.define _delvar               $475F   ; DELETE VAR
;
;  ASM MODULE EXECUTOR
;
.define _exec_assembly        $5730
;
;  RAM  ATES FOR ASM MODULE CALLING
;
.define _asm_reg_af              $D624
.define _asm_reg_a               $D625
.define _asm_reg_l               $D626
.define _asm_reg_hl              $D626
.define _asm_reg_h               $D627
.define _asm_reg_bc              $D628
.define _asm_reg_c               $D628
.define _asm_reg_b               $D629
.define _asm_reg_de              $D62A
.define _asm_reg_e               $D62A
.define _asm_reg_d               $D62B
;
;
;       start address for all ASM programs
;
;
.define _asm_exec_ram                       $D748
;
;
;       op1 to op6 ram  ates
;
;
.define _OP1                                $C089
.define _OP1EXPM                            $C08A
.define _OP1EXPL                            $C08B
.define _OP1M                               $C08C
.define _OP1EXT                             $C093
.define _OP2                                $C094
.define _OP2EXPM                            $C095
.define _OP2EXPL                            $C096
.define _OP2M                               $C097
.define _OP2EXT                             $C09E
.define _OP3                                $C09F
.define _OP3EXPM                            $C0A0
.define _OP3EXPL                            $C0A1
.define _OP3M                               $C0A2
.define _OP3EXT                             $C0A9
.define _OP4                                $C0AA
.define _OP4EXPM                            $C0AB
.define _OP4EXPL                            $C0AC
.define _OP4M                               $C0AD
.define _OP4EXT                             $C0B4
.define _OP5                                $C0B5
.define _OP5EXPM                            $C0B6
.define _OP5EXPL                            $C0B7
.define _OP5M                               $C0B8
.define _OP5EXT                             $C0BF
.define _OP6                                $C0C0
.define _OP6EXPM                            $C0C1
.define _OP6EXPL                            $C0C2
.define _OP6M                               $C0C3
.define _OP6EXT                             $C0CA
;
;
;       screen shadow ram  ates
;
;
.define _textShadow                         $C0F9
.define _cmdShadow                          $CFAB
.define _plotSScreen                        $C9FA
;
;
;       display cursor ram  ates
;
;
.define _curRow                             $C00F
.define _curCol                             $C010
.define _penCol                             $C37C
.define _penRow                             $C37D
;
;
;       link port  ates
;
;
.define BPort                 7               ;4-bit I/O
.define D0LD1L	        	     $0FC
.define D0LD1H	        	     $0D4
.define D0HD1L	        	     $0E8
.define D0HD1H	        	     $0C0
;
;       SYSTEM FLAG  ATES
;       ALL OF THESE FLAGS CAN BE ACCESSED THROUGH THE 'IY' REGISTER
;
.define trigflags               0           ; IY OFFSET VALUE
.define trigdeg                 2
;
.define plotflags               2           ; IY OFFSET VALUE
.define plotloc                 1
.define plotdisp                2
;
.define grfmodeflags            2           ; IY OFFSET VALUE
.define grffuncm                4
.define grfpolarm               5
.define grfparamm               6
.define grfrecurm               7
;
.define graphflags              3           ; IY OFFSET VALUE
.define graphdraw               0
.define graphcursor             2
;
.define grfdbflags              4           ; IY OFFSET VALUE
.define grfdot                  0
.define grfsimul                1
.define grfgrid                 2
.define grfpolar                3
.define grfnocoord              4
.define grfnoaxis               5
.define grflabel                6
;
.define textflags               5           ; IY OFFSET VALUE
.define textEraseBelow          1
.define textScrolled            2
.define textInverse             3
;
.define onflags                 9           ; IY OFFSET VALUE
.define onRunning               3
.define onInterrupt             4
;
.define fmtflags                10          ; IY OFFSET VALUE
.define fmtExponent             0
.define fmtEng                  1
;
.define curflags                12          ; IY OFFSET VALUE
.define curAble                 2
.define curOn                   3
.define curLock                 4
;
.define appflags                13          ; IY OFFSET VALUE
.define appTextSave             1
.define appAutoScroll           2
;
.define indicflags              18          ; IY OFFSET VALUE
.define indicRun                0
.define indicOnly               2
;
.define shiftflags              18          ; IY OFFSET VALUE
.define shift2nd                3
.define shiftAlpha              4
.define shiftLwrAlph            5
.define shiftALock              6
;
.define new_grf_flgs            24          ; IY OFFSET VALUE
.define textwrite               6
;
;
;    character font  ates
;
;
.define Lbinary   	$001
.define Loctal	   	$002
.define Ldecimal  	$003
.define Lhexadecimal   	$004
.define Lconvert  	$005
.define LsqUp  	 	$006
.define LsqDown   	$007
.define Lintegral	$008
.define Lcross	   	$009
.define LhexA  		$00A
.define LhexB  		$00B
.define LhexC  		$00C
.define LhexD  		$00D
.define LhexE  		$00E
.define LhexF  		$00F
.define Lroot  		$010
.define Linverse  	$011
.define Lsquare   	$012
.define Langle	   	$013
.define Ldegree   	$014
.define Lradian   	$015
.define Ltranspose   	$016
.define LLE	   	$017
.define LNE	   	$018
.define LGE	   	$019
.define Lneg   		$01A
.define Lexponent	$01B
.define Lstore	   	$01C
.define Lten   		$01D
.define LupArrow  	$01E
.define LdownArrow   	$01F
.define Lspace	   	$020
.define Lexclam   	$021
.define Lquote	   	$022
.define Lpound	   	$023
.define Ldollar   	$024
.define Lpercent  	$025
.define Lampersand   	$026
.define Lapostrophe  	$027
.define LlParen   	$028
.define LrParen   	$029
.define Lasterisk   	$02A
.define LplusSign   	$02B
.define Lcomma	   	$02C
.define Ldash  		$02D
.define Lperiod   	$02E
.define Lslash	   	$02F
.define L0	  	$030
.define L1	  	$031
.define L2	  	$032
.define L3	  	$033
.define L4	  	$034
.define L5	  	$035
.define L6	  	$036
.define L7	  	$037
.define L8	  	$038
.define L9	  	$039
.define Lcolon	   	$03A
.define Lsemicolon   	$03B
.define LLT	   	$03C
.define LEQ	   	$03D
.define LGT	   	$03E
.define Lquestion   	$03F
.define LatSign   	$040
.define LcapA  		$041
.define LcapB  		$042
.define LcapC  		$043
.define LcapD  		$044
.define LcapE  		$045
.define LcapF  		$046
.define LcapG  		$047
.define LcapH  		$048
.define LcapI  		$049
.define LcapJ  		$04A
.define LcapK  		$04B
.define LcapL  		$04C
.define LcapM  		$04D
.define LcapN  		$04E
.define LcapO  		$04F
.define LcapP  		$050
.define LcapQ  		$051
.define LcapR  		$052
.define LcapS  		$053
.define LcapT  		$054
.define LcapU  		$055
.define LcapV  		$056
.define LcapW  		$057
.define LcapX  		$058
.define LcapY  		$059
.define LcapZ  		$05A
.define LlBrack   	$05B
.define Lbackslash   	$05C
.define LrBrack   	$05D
.define Lcaret	   	$05E
.define Lunderscore  	$05F
.define Lbackquote   	$060
.define La  		$061
.define Lb  		$062
.define Lc  		$063
.define Ld  		$064
.define Le  		$065
.define Lf  		$066
.define Lg  		$067
.define Lh  		$068
.define Li  		$069
.define Lj  		$06A
.define Lk  		$06B
.define Ll  		$06C
.define Lm  		$06D
.define Ln  		$06E
.define Lo  		$06F
.define Lp  		$070
.define Lq  		$071
.define Lr  		$072
.define Ls  		$073
.define Lt  		$074
.define Lu  		$075
.define Lv  		$076
.define Lw  		$077
.define Lx  		$078
.define Ly  		$079
.define Lz  		$07A
.define LlBrace   	$07B
.define Lbar   		$07C
.define LrBrace   	$07D
.define Ltilde	   	$07E
.define LinvEQ	   	$07F
.define Lsub0  		$080
.define Lsub1  		$081
.define Lsub2  		$082
.define Lsub3  		$083
.define Lsub4  		$084
.define Lsub5  		$085
.define Lsub6  		$086
.define Lsub7  		$087
.define Lsub8  		$088
.define Lsub9  		$089
.define LcapAAcute   	$08A
.define LcapAGrave   	$08B
.define LcapACaret   	$08C
.define LcapADier   	$08D
.define LaAcute   	$08E
.define LaGrave   	$08F
.define LaCaret   	$090
.define LaDier	   	$091
.define LcapEAcute   	$092
.define LcapEGrave   	$093
.define LcapECaret   	$094
.define LcapEDier   	$095
.define LeAcute   	$096
.define LeGrave   	$097
.define LeCaret   	$098
.define LeDier	   	$099
.define LcapIAcute   	$09A
.define LcapIGrave   	$09B
.define LcapICaret   	$09C
.define LcapIDier  	$09D
.define LiAcute   	$09E
.define LiGrave   	$09F
.define LiCaret   	$0A0
.define LiDier	   	$0A1
.define LcapOAcute   	$0A2
.define LcapOGrave   	$0A3
.define LcapOCaret   	$0A4
.define LcapODier   	$0A5
.define LoAcute   	$0A6
.define LoGrave   	$0A7
.define LoCaret   	$0A8
.define LoDier	   	$0A9
.define LcapUAcute   	$0AA
.define LcapUGrave   	$0AB
.define LcapUCaret   	$0AC
.define LcapUDier   	$0AD
.define LuAcute   	$0AE
.define LuGrave   	$0AF
.define LuCaret   	$0B0
.define LuDier	   	$0B1
.define LcapCCed  	$0B2
.define LcCed  		$0B3
.define LcapNTilde   	$0B4
.define LnTilde   	$0B5
.define Laccent   	$0B6
.define Lgrave	   	$0B7
.define Ldieresis  	$0B8
.define LquesDown   	$0B9
.define LexclamDown  	$0BA
.define Lalpha	   	$0BB
.define Lbeta  		$0BC
.define Lgamma	   	$0BD
.define LcapDelta   	$0BE
.define Ldelta	   	$0BF
.define Lepsilon  	$0C0
.define Ltheta	   	$0C1
.define Llambda   	$0C2
.define Lmu	   	$0C3
.define Lpi	   	$0C4
.define Lrho   		$0C5
.define LcapSigma   	$0C6
.define Lsigma	   	$0C7
.define Ltau   		$0C8
.define Lphi   		$0C9
.define LcapOmega  	$0CA
.define LxMean	   	$0CB
.define LyMean	   	$0CC
.define LsupX  		$0CD
.define Lellipsis   	$0CE
.define Lleft  		$0CF
.define Lblock	   	$0D0
.define Lper   		$0D1
.define Lhyphen   	$0D2
.define Larea  		$0D3
.define Ltemp  		$0D4
.define Lcube  		$0D5
.define Lenter	   	$0D6
.define LblockArrow     $0D7
;
;
;       'getkey'  ates
;
;
.define kRight	   	$001
.define kLeft  		$002
.define kUp	   	$003
.define kDown  		$004
.define kColon	   	$005
.define kEnter	   	$006
.define kExit  		$007
.define kClear	   	$008
.define kDel   		$009
.define kIns   		$00A
.define kNext  		$00B
.define kAdd   		$00C
.define kSub   		$00D
.define kMul   		$00E
.define kDiv   		$00F
.define kExpon	   	$010
.define kLParen   	$011
.define kRParen   	$012
.define kLBrack   	$013
.define kRBrack   	$014
.define kEqual	 	$015
.define kStore	   	$016
.define kRecall   	$017
.define kComma	   	$018
.define kAng   		$019
.define kChs   		$01A
.define kDecPnt   	$01B
.define k0  		$01C
.define k1  		$01D
.define k2  		$01E
.define k3  		$01F
.define k4  		$020
.define k5  		$021
.define k6  		$022
.define k7  		$023
.define k8  		$024
.define k9  		$025
.define kEE	   	$026
.define kSpace	   	$027
.define kCapA  		$028
.define kCapB  		$029
.define kCapC  		$02A
.define kCapD  		$02B
.define kCapE  		$02C
.define kCapF  		$02D
.define kCapG  		$02E
.define kCapH  		$02F
.define kCapI  		$030
.define kCapJ  		$031
.define kCapK  		$032
.define kCapL  		$033
.define kCapM  		$034
.define kCapN  		$035
.define kCapO  		$036
.define kCapP  		$037
.define kCapQ  		$038
.define kCapR  		$039
.define kCapS  		$03A
.define kCapT  		$03B
.define kCapU  		$03C
.define kCapV  		$03D
.define kCapW  		$03E
.define kCapX  		$03F
.define kCapY  		$040
.define kCapZ  		$041
.define ka  		$042
.define kb  		$043
.define kc  		$044
.define kd  		$045
.define ke  		$046
.define kf  		$047
.define kg  		$048
.define kh  		$049
.define ki  		$04A
.define kj  		$04B
.define kk  		$04C
.define kl  		$04D
.define km  		$04E
.define kn  		$04F
.define ko  		$050
.define kp  		$051
.define kq  		$052
.define kr  		$053
.define ks  		$054
.define kt  		$055
.define ku  		$056
.define kv  		$057
.define kw  		$058
.define kx  		$059
.define ky  		$05A
.define kz  		$05B
.define kVarx  		$05C
.define kAns   		$05D
.define kPi	   	$05E
.define kInv   		$05F
.define kSin   		$060
.define kASin  		$061
.define kCos   		$062
.define kACos  		$063
.define kTan   		$064
.define kATan  		$065
.define kSquare   	$066
.define kSqrt  		$067
.define kLn	   	$068
.define kExp   		$069
.define kLog   		$06A
.define kALog  		$06B
.define kMath  		$06C
.define kCplx  		$06D
.define kString   	$06E
.define kTest  		$06F
.define kConv  		$070
.define kChar  		$071
.define kBase  		$072
.define kCustom   	$073
.define kVars  		$074
.define kCatalog  	$075
.define kQuit  		$076
.define kLastEnt  	$077
.define kLinkIO   	$078
.define kMem   		$079
.define kList  		$07A
.define kVector   	$07B
.define kConst	   	$07C
.define kMatrix   	$07D
.define kPoly  		$07E
.define kSimult   	$07F
.define kStat  		$080
.define kGrMenu   	$081
.define kMode  		$082
.define kPrgm  		$083
.define kCalcu	   	$084
.define kSolver   	$085
;
.define kTable	   	$086		; new key added for LEO
;
.define kBOL   		$087
.define kEOL   		$088
.define kCUSTM	   	$093
.define kF1	   	$0C2
.define kF2	   	$0C3
.define kF3	   	$0C4
.define kF4	   	$0C5
.define kF5	   	$0C6
.define kF6	   	$0C7
.define kF7	   	$0C8
.define kF8	   	$0C9
.define kF9	   	$0CA
.define kF10            $0CB
.define kGRAPH	   	$0FF
