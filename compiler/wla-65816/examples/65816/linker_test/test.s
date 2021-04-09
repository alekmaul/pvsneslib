
.DEFINE TESB 1
.EXPORT TESB

.SECTION "test"
test_00:	NOP
.ENDS

.PRINTT "what's the name of this program? "
.INPUT INFO

.SECTION "information"
.DB INFO
.ENDS
