// make sure these enums are syncronized with the ones in worker.ts
export var Action;
(function (Action) {
    Action[Action["StartQuantization"] = 0] = "StartQuantization";
    Action[Action["UpdateProgress"] = 1] = "UpdateProgress";
    Action[Action["UpdateQuantizedImage"] = 2] = "UpdateQuantizedImage";
    Action[Action["UpdatePalettes"] = 3] = "UpdatePalettes";
    Action[Action["DoneQuantization"] = 4] = "DoneQuantization";
})(Action || (Action = {}));
export var ColorZeroBehaviour;
(function (ColorZeroBehaviour) {
    ColorZeroBehaviour[ColorZeroBehaviour["Unique"] = 0] = "Unique";
    ColorZeroBehaviour[ColorZeroBehaviour["Shared"] = 1] = "Shared";
    ColorZeroBehaviour[ColorZeroBehaviour["TransparentFromTransparent"] = 2] = "TransparentFromTransparent";
    ColorZeroBehaviour[ColorZeroBehaviour["TransparentFromColor"] = 3] = "TransparentFromColor";
})(ColorZeroBehaviour || (ColorZeroBehaviour = {}));
export var Dither;
(function (Dither) {
    Dither[Dither["Off"] = 0] = "Off";
    Dither[Dither["Fast"] = 1] = "Fast";
    Dither[Dither["Slow"] = 2] = "Slow";
})(Dither || (Dither = {}));
export var DitherPattern;
(function (DitherPattern) {
    DitherPattern[DitherPattern["Diagonal4"] = 0] = "Diagonal4";
    DitherPattern[DitherPattern["Horizontal4"] = 1] = "Horizontal4";
    DitherPattern[DitherPattern["Vertical4"] = 2] = "Vertical4";
    DitherPattern[DitherPattern["Diagonal2"] = 3] = "Diagonal2";
    DitherPattern[DitherPattern["Horizontal2"] = 4] = "Horizontal2";
    DitherPattern[DitherPattern["Vertical2"] = 5] = "Vertical2";
})(DitherPattern || (DitherPattern = {}));
//# sourceMappingURL=enums.js.map