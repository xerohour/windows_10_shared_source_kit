    // #0 SRC0 = DC_INFn  (p = positive, n = negative)
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

    // #1 SRC0 = DC_DENORMn
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_INFn ,	DC_REALp,	DC_REALn,	DC_ONEn ,	DC_REALp,	DC_REALp,	DC_ONEp ,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_INFn ,	DC_ZEROp,	DC_REALn,	DC_ONEn ,	DC_DENORMp,	DC_DENORMp,	DC_ONEp ,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_DENORMn,	DC_DENORMn,	DC_ONEp ,	DC_REALp,	DC_DENORMn,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROn,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROn,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

    // #2 SRC0 = DC_REALn
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_INFn ,	DC_REALp,	DC_REALn,	DC_ONEn ,	DC_REALp,	DC_REALp,	DC_ONEp ,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_INFn ,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_INFn ,	DC_REALp,	DC_ZEROp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_REALn,	DC_REALn,	DC_REALn,	DC_REALn,	DC_REALn,	DC_REALn,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_REALn,	DC_REALn,	DC_ONEp ,	DC_REALn,	DC_REALn,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_REALn,	DC_REALn,	DC_ONEp ,	DC_REALn,	DC_REALn,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

    // #3 SRC0 = DC_ONEn 
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_INFn ,	DC_ZEROp,	DC_REALn,	DC_ONEn ,	DC_DENORMp,	DC_DENORMp,	DC_ONEp ,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_INFn ,	DC_REALp,	DC_ZEROp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_INFn ,	DC_ONEp ,	DC_REALn,	DC_ZEROp,	DC_ONEp ,	DC_ONEp ,	DC_REALp,	DC_ONEp ,	DC_ONEp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_INFn ,	DC_ONEn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_ONEn ,	DC_ZEROp,	DC_ONEn ,	DC_ONEn ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_REALn,	DC_REALn,	DC_ONEp ,	DC_ZEROp,	DC_REALn,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_DENORMn,	DC_DENORMn,	DC_ONEp ,	DC_REALp,	DC_ZEROp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

    // #4 SRC0 = DC_ZEROn
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

    // #5 SRC0 = DC_ZEROp
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

    // #6 SRC0 = DC_ONEp 
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_DENORMn,	DC_DENORMn,	DC_ONEp ,	DC_REALp,	DC_DENORMn,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_REALn,	DC_REALn,	DC_REALn,	DC_REALn,	DC_REALn,	DC_REALn,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_INFn ,	DC_ONEn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_ONEn ,	DC_ZEROp,	DC_ONEn ,	DC_ONEn ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_INFn ,	DC_ONEp ,	DC_REALn,	DC_ZEROp,	DC_ONEp ,	DC_ONEp ,	DC_REALp,	DC_ONEp ,	DC_ONEp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_INFn ,	DC_REALp,	DC_REALn,	DC_ONEn ,	DC_REALp,	DC_REALp,	DC_ONEp ,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_DENORMp,	DC_DENORMp,	DC_ONEp ,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

    // #7 SRC0 = DC_REALp
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROn,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_REALn,	DC_REALn,	DC_ONEp ,	DC_REALn,	DC_REALn,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_REALn,	DC_REALn,	DC_ONEp ,	DC_ZEROp,	DC_REALn,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_INFn ,	DC_REALp,	DC_REALn,	DC_ONEn ,	DC_REALp,	DC_REALp,	DC_ONEp ,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

    // #8 SRC0 = DC_DENORMp
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROn,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_REALn,	DC_REALn,	DC_ONEp ,	DC_REALn,	DC_REALn,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_INFn ,	DC_REALn,	DC_REALn,	DC_ONEn ,	DC_DENORMn,	DC_DENORMn,	DC_ONEp ,	DC_REALp,	DC_ZEROp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROn,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_DENORMp,	DC_DENORMp,	DC_ONEp ,	DC_REALp,	DC_REALp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_INFn ,	DC_DENORMn,	DC_REALn,	DC_ONEn ,	DC_ZEROp,	DC_ZEROp,	DC_ONEp ,	DC_REALp,	DC_DENORMp,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

    // #9 SRC0 = DC_INFp 
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_INFn ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_NAN  ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_INFp ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

    // #10 SRC0 = DC_NAN  
    //					 SRC2 = DC_INFn 	DC_DENORMn	DC_REALn	DC_ONEn 	DC_ZEROn	DC_ZEROp	DC_ONEp 	DC_REALp	DC_DENORMn	DC_INFp 	DC_NAN
	{
   /* SRC1 = DC_INFn 	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_DENORMn	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_REALn	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_ONEn 	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_ZEROn	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_ZEROp	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_ONEp 	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_REALp	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_DENORMp	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_INFp 	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
   /* SRC1 = DC_NAN  	*/ { 	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  ,	DC_NAN  , },
	},

