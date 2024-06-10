"use strict";
// we can't import these enums from enums.js, because worker modules are not supported in Firefox
var Action;
(function (Action) {
    Action[Action["StartQuantization"] = 0] = "StartQuantization";
    Action[Action["UpdateProgress"] = 1] = "UpdateProgress";
    Action[Action["UpdateQuantizedImage"] = 2] = "UpdateQuantizedImage";
    Action[Action["UpdatePalettes"] = 3] = "UpdatePalettes";
    Action[Action["DoneQuantization"] = 4] = "DoneQuantization";
})(Action || (Action = {}));
var ColorZeroBehaviour;
(function (ColorZeroBehaviour) {
    ColorZeroBehaviour[ColorZeroBehaviour["Unique"] = 0] = "Unique";
    ColorZeroBehaviour[ColorZeroBehaviour["Shared"] = 1] = "Shared";
    ColorZeroBehaviour[ColorZeroBehaviour["TransparentFromTransparent"] = 2] = "TransparentFromTransparent";
    ColorZeroBehaviour[ColorZeroBehaviour["TransparentFromColor"] = 3] = "TransparentFromColor";
})(ColorZeroBehaviour || (ColorZeroBehaviour = {}));
var Dither;
(function (Dither) {
    Dither[Dither["Off"] = 0] = "Off";
    Dither[Dither["Fast"] = 1] = "Fast";
    Dither[Dither["Slow"] = 2] = "Slow";
})(Dither || (Dither = {}));
var DitherPattern;
(function (DitherPattern) {
    DitherPattern[DitherPattern["Diagonal4"] = 0] = "Diagonal4";
    DitherPattern[DitherPattern["Horizontal4"] = 1] = "Horizontal4";
    DitherPattern[DitherPattern["Vertical4"] = 2] = "Vertical4";
    DitherPattern[DitherPattern["Diagonal2"] = 3] = "Diagonal2";
    DitherPattern[DitherPattern["Horizontal2"] = 4] = "Horizontal2";
    DitherPattern[DitherPattern["Vertical2"] = 5] = "Vertical2";
})(DitherPattern || (DitherPattern = {}));
const ditherPatterns = new Map();
ditherPatterns.set(DitherPattern.Diagonal4, [
    [0, 2],
    [3, 1],
]);
ditherPatterns.set(DitherPattern.Horizontal4, [
    [0, 3],
    [1, 2],
]);
ditherPatterns.set(DitherPattern.Vertical4, [
    [0, 1],
    [3, 2],
]);
ditherPatterns.set(DitherPattern.Diagonal2, [
    [0, 1],
    [1, 0],
]);
ditherPatterns.set(DitherPattern.Horizontal2, [
    [0, 1],
    [0, 1],
]);
ditherPatterns.set(DitherPattern.Vertical2, [
    [0, 0],
    [1, 1],
]);
let ditherPattern = ditherPatterns.get(DitherPattern.Diagonal4);
let ditherPixels = 4;
let quantizationOptions = {
    tileWidth: 8,
    tileHeight: 8,
    numPalettes: 8,
    colorsPerPalette: 4,
    bitsPerChannel: 5,
    fractionOfPixels: 0.1,
    colorZeroBehaviour: ColorZeroBehaviour.Unique,
    colorZeroValue: [0, 0, 0],
    dither: Dither.Off,
    ditherWeight: 0.5,
    ditherPattern: DitherPattern.Diagonal4,
};
onmessage = function (event) {
    updateProgress(0);
    const data = event.data;
    quantizationOptions = data.quantizationOptions;
    ditherPattern = ditherPatterns.get(quantizationOptions.ditherPattern);
    const patternPixels2 = new Set([
        DitherPattern.Diagonal2,
        DitherPattern.Horizontal2,
        DitherPattern.Vertical2,
    ]);
    if (patternPixels2.has(quantizationOptions.ditherPattern)) {
        ditherPixels = 2;
    }
    quantizeImage(data.imageData);
    updateProgress(100);
    postMessage({ action: Action.DoneQuantization });
};
function updateProgress(progress) {
    postMessage({ action: Action.UpdateProgress, progress: progress });
}
function updateQuantizedImage(image) {
    postMessage({ action: Action.UpdateQuantizedImage, imageData: image });
}
function updatePalettes(palettes, doSorting) {
    let pal = structuredClone(palettes);
    const colorZeroBehaviour = quantizationOptions.colorZeroBehaviour;
    let startIndex = 0;
    if (colorZeroBehaviour === ColorZeroBehaviour.TransparentFromColor ||
        colorZeroBehaviour === ColorZeroBehaviour.TransparentFromTransparent) {
        startIndex = 1;
        for (const palette of pal) {
            palette.unshift(cloneColor(quantizationOptions.colorZeroValue));
        }
    }
    if (colorZeroBehaviour === ColorZeroBehaviour.Shared) {
        startIndex = 1;
    }
    if (doSorting) {
        pal = sortPalettes(pal, startIndex);
    }
    postMessage({
        action: Action.UpdatePalettes,
        palettes: pal,
        numPalettes: quantizationOptions.numPalettes,
        numColors: quantizationOptions.colorsPerPalette,
    });
}
function movePalettesCloser(palettes, pixel, alpha) {
    let sharedColorIndex = -1;
    if (quantizationOptions.colorZeroBehaviour === ColorZeroBehaviour.Shared) {
        sharedColorIndex = 0;
    }
    let closestPaletteIndex = -1;
    let closestColorIndex = -1;
    let targetColor;
    if (quantizationOptions.dither === Dither.Slow) {
        closestPaletteIndex = getClosestPaletteIndexDither(palettes, pixel.tile);
        [closestColorIndex, , targetColor] = getClosestColorDither(palettes[closestPaletteIndex], pixel);
    }
    else {
        closestPaletteIndex = getClosestPaletteIndex(palettes, pixel.tile);
        [closestColorIndex] = getClosestColor(palettes[closestPaletteIndex], pixel.color);
        targetColor = pixel.color;
    }
    if (closestColorIndex !== sharedColorIndex) {
        moveColorCloser(palettes[closestPaletteIndex][closestColorIndex], targetColor, alpha);
    }
}
function quantizeImage(image) {
    console.log(quantizationOptions);
    const t0 = performance.now();
    const reducedImageData = {
        width: image.width,
        height: image.height,
        data: new Uint8ClampedArray(image.data.length),
    };
    const useDither = quantizationOptions.dither !== Dither.Off;
    if (useDither) {
        for (let i = 0; i < image.data.length; i++) {
            reducedImageData.data[i] = image.data[i];
        }
    }
    else {
        for (let i = 0; i < image.data.length; i++) {
            reducedImageData.data[i] = toNbit(image.data[i], quantizationOptions.bitsPerChannel);
        }
    }
    const tiles = extractTiles(reducedImageData);
    let avgPixelsPerTile = 0;
    for (const tile of tiles) {
        avgPixelsPerTile += tile.colors.length;
    }
    avgPixelsPerTile /= tiles.length;
    console.log("Colors per tile: " + avgPixelsPerTile.toFixed(2));
    const pixels = extractAllPixels(tiles);
    const randomShuffle = new RandomShuffle(pixels.length);
    const showProgress = true;
    let iterations = quantizationOptions.fractionOfPixels * pixels.length;
    let alpha = 0.3;
    let finalAlpha = 0.05;
    const meanSquareErr = meanSquareError;
    if (quantizationOptions.dither === Dither.Slow) {
        // meanSquareErr = meanSquareErrorDither;
        iterations /= 5;
        alpha = 0.1;
        finalAlpha = 0.02;
    }
    const minColorFactor = 0.5;
    const minPaletteFactor = 0.5;
    const replaceIterations = 10;
    const useMin = true;
    const prog = [25, 65, 90, 100];
    if (quantizationOptions.dither != Dither.Off) {
        prog[3] = 94;
    }
    let palettes = colorQuantize1Color(tiles, pixels, randomShuffle);
    let startIndex = 2;
    if (quantizationOptions.colorZeroBehaviour === ColorZeroBehaviour.Shared) {
        startIndex += 1;
    }
    let endIndex = quantizationOptions.colorsPerPalette;
    if (quantizationOptions.colorZeroBehaviour ===
        ColorZeroBehaviour.TransparentFromColor ||
        quantizationOptions.colorZeroBehaviour ===
            ColorZeroBehaviour.TransparentFromTransparent) {
        endIndex -= 1;
    }
    updateProgress(prog[0] / quantizationOptions.numPalettes);
    updatePalettes(palettes, false);
    if (showProgress)
        updateQuantizedImage(quantizeTiles(palettes, reducedImageData, false));
    for (let numColors = startIndex; numColors <= endIndex; numColors++) {
        expandPalettesByOneColor(palettes, tiles, pixels, randomShuffle);
        updateProgress((prog[0] * numColors) / quantizationOptions.colorsPerPalette);
        updatePalettes(palettes, false);
        if (showProgress)
            updateQuantizedImage(quantizeTiles(palettes, reducedImageData, false));
    }
    let minMse = meanSquareErr(palettes, tiles);
    let minPalettes = structuredClone(palettes);
    for (let i = 0; i < replaceIterations; i++) {
        palettes = replaceWeakestColors(palettes, tiles, minColorFactor, minPaletteFactor, true);
        for (let iteration = 0; iteration < iterations; iteration++) {
            const nextPixel = pixels[randomShuffle.next()];
            movePalettesCloser(palettes, nextPixel, alpha);
        }
        const mse = meanSquareErr(palettes, tiles);
        if (mse < minMse) {
            minMse = mse;
            minPalettes = structuredClone(palettes);
        }
        updateProgress(prog[0] + ((prog[1] - prog[0]) * (i + 1)) / replaceIterations);
        updatePalettes(palettes, false);
        if (showProgress) {
            if (useMin && i === replaceIterations - 1) {
                updateQuantizedImage(quantizeTiles(minPalettes, reducedImageData, false));
            }
            else {
                updateQuantizedImage(quantizeTiles(palettes, reducedImageData, false));
            }
        }
        console.log("MSE: " + mse.toFixed(0));
        // console.log((performance.now() - t1).toFixed(0) + " ms");
    }
    if (useMin) {
        palettes = minPalettes;
    }
    if (!useDither)
        palettes = reducePalettes(palettes, quantizationOptions.bitsPerChannel);
    const finalIterations = iterations * 10;
    let nextUpdate = iterations;
    for (let iteration = 0; iteration < finalIterations; iteration++) {
        const nextPixel = pixels[randomShuffle.next()];
        movePalettesCloser(palettes, nextPixel, finalAlpha);
        if (iteration >= nextUpdate) {
            nextUpdate += iterations;
            updateProgress(prog[1] + ((prog[2] - prog[1]) * iteration) / finalIterations);
            updatePalettes(palettes, false);
        }
    }
    console.log("Normal final: " + meanSquareError(palettes, tiles).toFixed(0));
    console.log("Dither final: " + meanSquareErrorDither(palettes, tiles).toFixed(0));
    updateProgress(prog[2]);
    updatePalettes(palettes, false);
    if (!useDither) {
        palettes = reducePalettes(palettes, quantizationOptions.bitsPerChannel);
        for (let i = 0; i < 3; i++) {
            palettes = kMeans(palettes, tiles);
            updateProgress(prog[2] + ((prog[3] - prog[2]) * (i + 1)) / 3);
            updatePalettes(palettes, false);
        }
    }
    palettes = reducePalettes(palettes, quantizationOptions.bitsPerChannel);
    updatePalettes(palettes, true);
    updateQuantizedImage(quantizeTiles(palettes, reducedImageData, useDither));
    console.log("> MSE: " + meanSquareError(palettes, tiles).toFixed(2));
    console.log(`> Time: ${((performance.now() - t0) / 1000).toFixed(2)} sec`);
}
function reducePalettes(palettes, bitsPerChannel) {
    const result = [];
    for (const palette of palettes) {
        const pal = [];
        for (const color of palette) {
            const col = cloneColor(color);
            toNbitColor(col, bitsPerChannel);
            pal.push(col);
        }
        result.push(pal);
    }
    return result;
}
function sortPalettes(palettes, startIndex) {
    const pairIterations = 2000;
    const tIterations = 10000;
    const paletteIterations = 100000;
    const upWeight = 2;
    const numPalettes = palettes.length;
    const numColors = palettes[0].length;
    if (numColors === 2 && startIndex === 1) {
        return palettes;
    }
    // paletteDist[i+1][j+1] stores distance between palette i and palette j
    const paletteDist = zeros2(numPalettes + 2, numPalettes + 2);
    // colorIndex[p1][p2][i] stores the index of the closest color in p2 from color index i in p1
    const colorIndex = zeros3(numPalettes, numPalettes, numColors);
    for (let i = 0; i < numPalettes; i++) {
        for (let j = 0; j < numPalettes; j++) {
            for (let k = 0; k < numColors; k++) {
                colorIndex[i][j][k] = k;
            }
        }
    }
    for (let p1 = 0; p1 < numPalettes - 1; p1++) {
        for (let p2 = p1 + 1; p2 < numPalettes; p2++) {
            const index = colorIndex[p1][p2];
            for (let iteration = 0; iteration < pairIterations; iteration++) {
                let i1 = startIndex +
                    Math.floor(Math.random() * (numColors - startIndex - 1));
                let i2 = i1 + 1 + Math.floor(Math.random() * (numColors - i1 - 1));
                if (Math.random() < 0.5) {
                    [i1, i2] = [i2, i1];
                }
                const p1i1 = palettes[p1][i1];
                const p1i2 = palettes[p1][i2];
                const p2i1 = palettes[p2][index[i1]];
                const p2i2 = palettes[p2][index[i2]];
                const straightDist = colorDistance(p1i1, p2i1) + colorDistance(p1i2, p2i2);
                const swappedDist = colorDistance(p1i1, p2i2) + colorDistance(p1i2, p2i1);
                if (swappedDist < straightDist) {
                    [index[i1], index[i2]] = [index[i2], index[i1]];
                }
            }
            let sum = 0;
            for (let i = 0; i < numColors; i++) {
                const p1i = palettes[p1][i];
                const p2i = palettes[p2][index[i]];
                sum += colorDistance(p1i, p2i);
            }
            paletteDist[p1 + 1][p2 + 1] = sum;
            paletteDist[p2 + 1][p1 + 1] = sum;
        }
    }
    for (let p1 = 1; p1 < numPalettes; p1++) {
        for (let p2 = 0; p2 < p1; p2++) {
            const index = colorIndex[p2][p1];
            const revIndex = colorIndex[p1][p2];
            for (let i = 0; i < numColors; i++) {
                revIndex[i] = index.indexOf(i);
            }
        }
    }
    const palIndex = [];
    for (let i = 0; i < numPalettes + 2; i++) {
        palIndex.push(i);
    }
    if (numPalettes > 2) {
        for (let iteration = 0; iteration < paletteIterations; iteration++) {
            const index1 = Math.max(1, Math.floor(Math.random() * numPalettes));
            const index2 = Math.min(numPalettes, index1 + 1 + Math.floor(Math.random() * numPalettes));
            const i1b = palIndex[index1 - 1];
            const i1 = palIndex[index1];
            const i2 = palIndex[index2];
            const i2b = palIndex[index2 + 1];
            const straightDist = paletteDist[i1b][i1] + paletteDist[i2][i2b];
            const swappedDist = paletteDist[i1b][i2] + paletteDist[i1][i2b];
            if (swappedDist < straightDist) {
                reverse(palIndex, index1, index2);
            }
        }
    }
    const pal1 = palettes[palIndex[1] - 1];
    const p1Index = [];
    for (let i = 0; i < numColors + 2; i++) {
        p1Index.push(i);
    }
    const p1Dist = zeros2(numColors + 2, numColors + 2);
    for (let i = 1; i <= numColors; i++) {
        for (let j = 1; j <= numColors; j++) {
            p1Dist[i][j] = colorDistance(pal1[i - 1], pal1[j - 1]);
        }
    }
    if (numColors > 2) {
        for (let iteration = 0; iteration < paletteIterations; iteration++) {
            const index1 = Math.max(1 + startIndex, Math.floor(Math.random() * numColors));
            const index2 = Math.min(numColors, index1 + 1 + Math.floor(Math.random() * numColors));
            const i1b = p1Index[index1 - 1];
            const i1 = p1Index[index1];
            const i2 = p1Index[index2];
            const i2b = p1Index[index2 + 1];
            const straightDist = p1Dist[i1b][i1] + p1Dist[i2][i2b];
            const swappedDist = p1Dist[i1b][i2] + p1Dist[i1][i2b];
            if (swappedDist < straightDist) {
                reverse(p1Index, index1, index2);
            }
        }
    }
    const pIndex = zeros2(numPalettes, numColors);
    for (let i = 0; i < numColors; i++) {
        pIndex[0][i] = p1Index[i + 1] - 1;
    }
    for (let i = 1; i < numPalettes; i++) {
        for (let j = 0; j < numColors; j++) {
            const p1 = palIndex[i] - 1;
            const p2 = palIndex[i + 1] - 1;
            pIndex[i][j] = colorIndex[p1][p2][pIndex[i - 1][j]];
        }
    }
    if (numColors >= 4)
        for (let i = 1; i < numPalettes; i++) {
            const p1 = palIndex[i] - 1;
            const p2 = palIndex[i + 1] - 1;
            let iteration = 0;
            while (iteration < tIterations) {
                const index1 = Math.max(startIndex, Math.floor(Math.random() * numColors));
                const index2 = Math.max(startIndex, Math.floor(Math.random() * numColors));
                if (index1 === index2)
                    continue;
                const up1 = pIndex[i - 1][index1];
                const i1 = pIndex[i][index1];
                const left1 = pIndex[i][index1 - 1];
                const right1 = pIndex[i][index1 + 1];
                const up2 = pIndex[i - 1][index2];
                const i2 = pIndex[i][index2];
                const left2 = pIndex[i][index2 - 1];
                const right2 = pIndex[i][index2 + 1];
                let straightDist = upWeight *
                    colorDistance(palettes[p2][i1], palettes[p1][up1]);
                if (left1 >= 0)
                    straightDist += colorDistance(palettes[p2][i1], palettes[p2][left1]);
                if (right1 < numColors)
                    straightDist += colorDistance(palettes[p2][i1], palettes[p2][right1]);
                straightDist +=
                    upWeight *
                        colorDistance(palettes[p2][i2], palettes[p1][up2]);
                if (left2 >= 0)
                    straightDist += colorDistance(palettes[p2][i2], palettes[p2][left2]);
                if (right2 < numColors)
                    straightDist += colorDistance(palettes[p2][i2], palettes[p2][right2]);
                let swappedDist = upWeight *
                    colorDistance(palettes[p2][i2], palettes[p1][up1]);
                if (left1 >= 0)
                    swappedDist += colorDistance(palettes[p2][i2], palettes[p2][left1]);
                if (right1 < numColors)
                    swappedDist += colorDistance(palettes[p2][i2], palettes[p2][right1]);
                swappedDist +=
                    upWeight *
                        colorDistance(palettes[p2][i1], palettes[p1][up2]);
                if (left2 >= 0)
                    swappedDist += colorDistance(palettes[p2][i1], palettes[p2][left2]);
                if (right2 < numColors)
                    swappedDist += colorDistance(palettes[p2][i1], palettes[p2][right2]);
                if (swappedDist < straightDist) {
                    [pIndex[i][index1], pIndex[i][index2]] = [
                        pIndex[i][index2],
                        pIndex[i][index1],
                    ];
                }
                iteration++;
            }
        }
    const pals = [];
    for (let i = 0; i < numPalettes; i++) {
        const p2 = palIndex[i + 1] - 1;
        const pal = [];
        for (let j = 0; j < numColors; j++) {
            pal.push(palettes[p2][pIndex[i][j]]);
        }
        pals.push(pal);
    }
    return pals;
}
function zeroArray(len) {
    const result = [];
    for (let i = 0; i < len; i++) {
        result.push(0);
    }
    return result;
}
function zeros2(len1, len2) {
    const result = [];
    for (let i = 0; i < len1; i++) {
        result.push(zeroArray(len2));
    }
    return result;
}
function zeros3(len1, len2, len3) {
    const result = [];
    for (let i = 0; i < len1; i++) {
        result.push(zeros2(len2, len3));
    }
    return result;
}
function reverse(a, left, right) {
    const middle = (left + right) / 2.0;
    while (left < middle) {
        [a[left], a[right]] = [a[right], a[left]];
        left++;
        right--;
    }
}
function toLinear(x) {
    return x * x;
}
function toLinearColor(color) {
    for (let i = 0; i < color.length; i++) {
        color[i] = toLinear(color[i]);
    }
}
function toSrgb(x) {
    return Math.sqrt(x);
}
function toSrgbColor(color) {
    for (let i = 0; i < color.length; i++) {
        color[i] = toSrgb(color[i]);
    }
}
const brightnessScale = [0.299, 0.587, 0.114];
function brightness(color) {
    let sum = 0;
    for (let i = 0; i < 3; i++) {
        sum += brightnessScale[i] * toLinear(color[i]);
    }
    return sum;
}
function replaceWeakestColors(palettes, tiles, minColorFactor, minPaletteFactor, replacePalettes) {
    const colorZeroBehaviour = quantizationOptions.colorZeroBehaviour;
    const useSlowDither = quantizationOptions.dither === Dither.Slow;
    let closestPal = closestPaletteDistance;
    if (useSlowDither) {
        closestPal = closestPaletteDistanceDither;
    }
    const closestPaletteIndex = zeroArray(tiles.length);
    let maxPaletteIndex = 0;
    let minPaletteIndex = 0;
    const totalPaletteMse = zeroArray(palettes.length);
    const removedPaletteMse = zeroArray(palettes.length);
    if (palettes.length > 1) {
        for (let j = 0; j < tiles.length; j++) {
            const tile = tiles[j];
            const [index, minDistance] = closestPal(palettes, tile);
            totalPaletteMse[index] += minDistance;
            closestPaletteIndex[j] = index;
            const remainingPalettes = [];
            for (let i = 0; i < palettes.length; i++) {
                if (i != index) {
                    remainingPalettes.push(palettes[i]);
                }
            }
            if (remainingPalettes.length > 0) {
                const [, minDistance2] = closestPal(remainingPalettes, tile);
                removedPaletteMse[index] += minDistance2;
            }
        }
        maxPaletteIndex = maxIndex(totalPaletteMse);
        minPaletteIndex = minIndex(removedPaletteMse);
    }
    const result = [];
    if (palettes[0].length > 1) {
        const totalColorMse = [];
        const secondColorMse = [];
        for (let j = 0; j < palettes.length; j++) {
            totalColorMse.push(zeroArray(palettes[j].length));
            secondColorMse.push(zeroArray(palettes[j].length));
        }
        for (let j = 0; j < tiles.length; j++) {
            const tile = tiles[j];
            const minPaletteIndex = closestPaletteIndex[j];
            const pal = palettes[minPaletteIndex];
            if (useSlowDither) {
                for (const pixel of tile.pixels) {
                    const [minColorIndex, minDist] = getClosestColorDither(pal, pixel);
                    totalColorMse[minPaletteIndex][minColorIndex] += minDist;
                    const remainingColors = [];
                    for (let i = 0; i < pal.length; i++) {
                        if (i != minColorIndex) {
                            remainingColors.push(pal[i]);
                        }
                    }
                    const [, secondDist] = getClosestColorDither(remainingColors, pixel);
                    secondColorMse[minPaletteIndex][minColorIndex] +=
                        secondDist;
                }
            }
            else {
                for (let i = 0; i < tile.colors.length; i++) {
                    const color = tile.colors[i];
                    const [minColorIndex, minDist] = getClosestColor(pal, color);
                    totalColorMse[minPaletteIndex][minColorIndex] +=
                        minDist * tile.counts[i];
                    const remainingColors = [];
                    for (let i = 0; i < pal.length; i++) {
                        if (i != minColorIndex) {
                            remainingColors.push(pal[i]);
                        }
                    }
                    const [, secondDist] = getClosestColor(remainingColors, color);
                    secondColorMse[minPaletteIndex][minColorIndex] +=
                        secondDist * tile.counts[i];
                }
            }
        }
        let sharedColorIndex = -1;
        if (colorZeroBehaviour === ColorZeroBehaviour.Shared) {
            sharedColorIndex = 0;
        }
        for (let palIndex = 0; palIndex < palettes.length; palIndex++) {
            const maxColorIndex = maxIndex(totalColorMse[palIndex]);
            const minColorIndex = minIndex(secondColorMse[palIndex]);
            const shouldReplaceMinColor = minColorIndex !== maxColorIndex &&
                minColorIndex !== sharedColorIndex &&
                secondColorMse[palIndex][minColorIndex] <
                    minColorFactor * totalColorMse[palIndex][maxColorIndex];
            const colors = [];
            for (let i = 0; i < palettes[palIndex].length; i++) {
                if (i == minColorIndex && shouldReplaceMinColor) {
                    console.log("replaced color in palette " + palIndex);
                    colors.push(cloneColor(palettes[palIndex][maxColorIndex]));
                }
                else {
                    colors.push(cloneColor(palettes[palIndex][i]));
                }
            }
            result.push(colors);
        }
    }
    else {
        for (let palIndex = 0; palIndex < palettes.length; palIndex++) {
            const colors = [];
            for (let i = 0; i < palettes[palIndex].length; i++) {
                colors.push(cloneColor(palettes[palIndex][i]));
            }
            result.push(colors);
        }
    }
    if (replacePalettes &&
        minPaletteIndex != maxPaletteIndex &&
        removedPaletteMse[minPaletteIndex] <
            minPaletteFactor * totalPaletteMse[maxPaletteIndex]) {
        console.log("replaced palette " + minPaletteIndex);
        while (result[minPaletteIndex].length > 0)
            result[minPaletteIndex].pop();
        for (const color of result[maxPaletteIndex]) {
            const c = structuredClone(color);
            result[minPaletteIndex].push(c);
        }
    }
    return result;
}
function kMeans(palettes, tiles) {
    const colorZeroBehaviour = quantizationOptions.colorZeroBehaviour;
    const counts = [];
    const sumColors = [];
    for (let i = 0; i < palettes.length; i++) {
        const c = [];
        const colors = [];
        for (let j = 0; j < palettes[i].length; j++) {
            c.push(0);
            colors.push([0, 0, 0]);
        }
        counts.push(c);
        sumColors.push(colors);
    }
    for (const tile of tiles) {
        if (quantizationOptions.dither === Dither.Slow) {
            const palIndex = getClosestPaletteIndexDither(palettes, tile);
            for (const pixel of tile.pixels) {
                const [colIndex, ,] = getClosestColorDither(palettes[palIndex], pixel);
                counts[palIndex][colIndex] += 1;
                addColor(sumColors[palIndex][colIndex], pixel.color);
            }
        }
        else {
            const palIndex = getClosestPaletteIndex(palettes, tile);
            for (let i = 0; i < tile.colors.length; i++) {
                const [colIndex] = getClosestColor(palettes[palIndex], tile.colors[i]);
                counts[palIndex][colIndex] += tile.counts[i];
                const color = cloneColor(tile.colors[i]);
                scaleColor(color, tile.counts[i]);
                addColor(sumColors[palIndex][colIndex], color);
            }
        }
    }
    let sharedColorIndex = -1;
    if (colorZeroBehaviour === ColorZeroBehaviour.Shared) {
        sharedColorIndex = 0;
    }
    for (let i = 0; i < sumColors.length; i++) {
        for (let j = 0; j < sumColors[i].length; j++) {
            if (counts[i][j] == 0 || j === sharedColorIndex) {
                sumColors[i][j] = cloneColor(palettes[i][j]);
            }
            else {
                scaleColor(sumColors[i][j], 1.0 / counts[i][j]);
            }
        }
    }
    return sumColors;
}
function meanSquareError(palettes, tiles) {
    let totalDistance = 0;
    let count = 0;
    for (const tile of tiles) {
        const palIndex = getClosestPaletteIndex(palettes, tile);
        for (let i = 0; i < tile.colors.length; i++) {
            const [, minDistance] = getClosestColor(palettes[palIndex], tile.colors[i]);
            totalDistance += minDistance * tile.counts[i];
            count += tile.counts[i];
        }
    }
    return totalDistance / count;
}
function meanSquareErrorDither(palettes, tiles) {
    let totalDistance = 0;
    let count = 0;
    for (const tile of tiles) {
        const palIndex = getClosestPaletteIndexDither(palettes, tile);
        for (const pixel of tile.pixels) {
            const [, minDistance] = getClosestColorDither(palettes[palIndex], pixel);
            totalDistance += minDistance;
            count += 1;
        }
    }
    return totalDistance / count;
}
class RandomShuffle {
    constructor(n) {
        this.values = [];
        for (let i = 0; i < n; i++) {
            this.values.push(i);
        }
        this.currentIndex = n - 1;
    }
    shuffle() {
        for (let i = 0; i < this.values.length; i++) {
            const index = i + Math.floor(Math.random() * (this.values.length - i));
            const tmp = this.values[i];
            this.values[i] = this.values[index];
            this.values[index] = tmp;
        }
    }
    next() {
        this.currentIndex += 1;
        if (this.currentIndex >= this.values.length) {
            this.shuffle();
            this.currentIndex = 0;
        }
        return this.values[this.currentIndex];
    }
}
function getClosestColor(palette, color) {
    let minIndex = palette.length - 1;
    let minDist = colorDistance(palette[minIndex], color);
    for (let i = palette.length - 2; i >= 0; i--) {
        const dist = colorDistance(palette[i], color);
        if (dist < minDist) {
            minIndex = i;
            minDist = dist;
        }
    }
    return [minIndex, minDist];
}
function getClosestColorDither(palette, pixel) {
    const error = [0, 0, 0];
    const linearPixel = cloneColor(pixel.color);
    toLinearColor(linearPixel);
    const candidates = [];
    const c = [0, 0, 0];
    const err = [0, 0, 0];
    const reducedColor = [0, 0, 0];
    for (let i = 0; i < ditherPixels; i++) {
        copyColor(c, linearPixel);
        copyColor(err, error);
        scaleColor(err, quantizationOptions.ditherWeight);
        addColor(c, err);
        clampColor(c, 0, 255 * 255);
        toSrgbColor(c);
        const [minColorIndex, minDist] = getClosestColor(palette, c);
        const minColor = palette[minColorIndex];
        candidates.push({
            colorIndex: minColorIndex,
            colorDistance: minDist,
            comparedColor: c,
            brightness: brightness(minColor),
        });
        copyColor(reducedColor, minColor);
        toNbitColor(reducedColor, quantizationOptions.bitsPerChannel);
        toLinearColor(reducedColor);
        addColor(error, linearPixel);
        subtractColor(error, reducedColor);
    }
    for (let i = 0; i < ditherPixels - 1; i++) {
        for (let j = i + 1; j < ditherPixels; j++) {
            if (candidates[i].brightness > candidates[j].brightness) {
                [candidates[i], candidates[j]] = [candidates[j], candidates[i]];
            }
        }
    }
    const index = ditherPattern[pixel.x & 1][pixel.y & 1];
    return [
        candidates[index].colorIndex,
        candidates[index].colorDistance,
        candidates[index].comparedColor,
    ];
}
function colorDistance(a, b) {
    return 2 * Math.pow((a[0] - b[0]), 2) + 4 * Math.pow((a[1] - b[1]), 2) + Math.pow((a[2] - b[2]), 2);
}
function paletteDistance(palette, tile) {
    let sum = 0;
    const colors = tile.colors;
    const counts = tile.counts;
    for (let i = 0; i < colors.length; i++) {
        const [, minDist] = getClosestColor(palette, colors[i]);
        sum += counts[i] * minDist;
    }
    return sum;
}
function paletteDistanceDither(palette, tile) {
    let sum = 0;
    for (const pixel of tile.pixels) {
        const [, minDist] = getClosestColorDither(palette, pixel);
        sum += minDist;
    }
    return sum;
}
function getClosestPaletteIndex(palettes, tile) {
    if (palettes.length === 1)
        return 0;
    const distances = palettes.map((palette) => paletteDistance(palette, tile));
    return minIndex(distances);
}
function closestPaletteDistance(palettes, tile) {
    const distances = palettes.map((palette) => paletteDistance(palette, tile));
    const index = minIndex(distances);
    return [index, distances[index]];
}
function getClosestPaletteIndexDither(palettes, tile) {
    if (palettes.length === 1)
        return 0;
    const distances = palettes.map((palette) => paletteDistanceDither(palette, tile));
    return minIndex(distances);
}
function closestPaletteDistanceDither(palettes, tile) {
    const distances = palettes.map((palette) => paletteDistanceDither(palette, tile));
    const index = minIndex(distances);
    return [index, distances[index]];
}
function getColor(image, x, y) {
    const index = 4 * (x + image.width * y);
    const color = [
        image.data[index],
        image.data[index + 1],
        image.data[index + 2],
    ];
    return color;
}
function extractTile(image, startX, startY) {
    const { tileWidth, tileHeight, colorZeroBehaviour, colorZeroValue } = quantizationOptions;
    const tile = {
        colors: [],
        counts: [],
        pixels: [],
    };
    const endX = Math.min(startX + tileWidth, image.width);
    const endY = Math.min(startY + tileHeight, image.height);
    for (let y = startY; y < endY; y++) {
        for (let x = startX; x < endX; x++) {
            const color = getColor(image, x, y);
            // skip transparent pixels
            if (isColorTransparent(color) || isPixelTransparent(x, y)) {
                continue;
            }
            tile.pixels.push({ tile, color, x, y });
            const colorIndex = tile.colors.findIndex((c) => equalColors(c, color));
            if (colorIndex >= 0) {
                tile.counts[colorIndex]++;
            }
            else {
                tile.colors.push(color);
                tile.counts.push(1);
            }
        }
    }
    return tile;
    function isPixelTransparent(x, y) {
        const index = 4 * (x + image.width * y);
        return (colorZeroBehaviour ===
            ColorZeroBehaviour.TransparentFromTransparent &&
            image.data[index + 3] < 255);
    }
    function isColorTransparent(color) {
        return (colorZeroBehaviour === ColorZeroBehaviour.TransparentFromColor &&
            equalColors(color, colorZeroValue));
    }
}
function extractTiles(image) {
    const { tileWidth, tileHeight } = quantizationOptions;
    const tiles = [];
    let totalPixels = 0;
    let tileCount = 0;
    for (let y = 0; y < image.height; y += tileHeight) {
        for (let x = 0; x < image.width; x += tileWidth) {
            const tile = extractTile(image, x, y);
            if (tile.colors.length === 0)
                continue;
            tiles.push(tile);
            totalPixels += tile.pixels.length;
            tileCount++;
        }
    }
    const avgPixelsPerTile = totalPixels / tileCount;
    console.log("avg pixels per tile: " + avgPixelsPerTile.toFixed(2));
    return tiles;
}
function equalColors(c1, c2) {
    for (let i = 0; i < c1.length; i++) {
        if (c1[i] !== c2[i]) {
            return false;
        }
    }
    return true;
}
function extractAllPixels(tiles) {
    const pixels = [];
    for (const tile of tiles) {
        for (const pixel of tile.pixels) {
            pixels.push(Object.assign({}, pixel));
        }
    }
    return pixels;
}
function quantizeTiles(palettes, image, useDither) {
    const { tileWidth, tileHeight, bitsPerChannel, colorZeroBehaviour, colorZeroValue, numPalettes, colorsPerPalette, } = quantizationOptions;
    const imageIsReduced = quantizationOptions.dither !== Dither.Off;
    let adjustedIndex = 0;
    if (colorZeroBehaviour === ColorZeroBehaviour.TransparentFromColor ||
        colorZeroBehaviour === ColorZeroBehaviour.TransparentFromTransparent) {
        adjustedIndex = 1;
    }
    const reducedPalettes = structuredClone(palettes);
    for (const pal of reducedPalettes) {
        for (const color of pal) {
            toNbitColor(color, bitsPerChannel);
        }
    }
    const transparentColor = cloneColor(colorZeroValue);
    if (imageIsReduced)
        toNbitColor(transparentColor, bitsPerChannel);
    const colorZero = cloneColor(colorZeroValue);
    toNbitColor(colorZero, bitsPerChannel);
    const bmpWidth = Math.ceil(image.width / 4) * 4;
    const quantizedImage = {
        width: image.width,
        height: image.height,
        data: new Uint8ClampedArray(image.data.length),
        totalPaletteColors: numPalettes * colorsPerPalette,
        paletteData: new Uint8ClampedArray(1024),
        colorIndexes: new Uint8ClampedArray(bmpWidth * image.height),
    };
    if (numPalettes * colorsPerPalette <= 256) {
        addBmpColors(reducedPalettes, quantizedImage.paletteData);
    }
    for (let startY = 0; startY < image.height; startY += tileHeight) {
        for (let startX = 0; startX < image.width; startX += tileWidth) {
            const tile = extractTile(image, startX, startY);
            let palette = reducedPalettes[0];
            let closestPaletteIndex = 0;
            if (tile.colors.length > 0) {
                if (useDither) {
                    closestPaletteIndex = getClosestPaletteIndexDither(reducedPalettes, tile);
                }
                else {
                    closestPaletteIndex = getClosestPaletteIndex(reducedPalettes, tile);
                }
                palette = reducedPalettes[closestPaletteIndex];
            }
            const endX = Math.min(startX + tileWidth, image.width);
            const endY = Math.min(startY + tileHeight, image.height);
            for (let y = startY; y < endY; y++) {
                for (let x = startX; x < endX; x++) {
                    const index = 4 * (x + image.width * y);
                    const bmpIndex = x + bmpWidth * (image.height - 1 - y);
                    const color = [
                        image.data[index],
                        image.data[index + 1],
                        image.data[index + 2],
                    ];
                    if ((colorZeroBehaviour ===
                        ColorZeroBehaviour.TransparentFromTransparent &&
                        image.data[index + 3] < 255) ||
                        (colorZeroBehaviour ===
                            ColorZeroBehaviour.TransparentFromColor &&
                            equalColors(color, transparentColor))) {
                        quantizedImage.data[index + 0] = image.data[index + 0];
                        quantizedImage.data[index + 1] = image.data[index + 1];
                        quantizedImage.data[index + 2] = image.data[index + 2];
                        quantizedImage.data[index + 3] = image.data[index + 3];
                        quantizedImage.colorIndexes[bmpIndex] =
                            closestPaletteIndex * colorsPerPalette;
                    }
                    else {
                        let closestColorIndex = 0;
                        if (useDither) {
                            [closestColorIndex] = getClosestColorDither(palette, {
                                color: color,
                                x: x,
                                y: y,
                            });
                        }
                        else {
                            [closestColorIndex] = getClosestColor(palette, color);
                        }
                        const paletteColor = cloneColor(palette[closestColorIndex]);
                        quantizedImage.data[index + 0] = paletteColor[0];
                        quantizedImage.data[index + 1] = paletteColor[1];
                        quantizedImage.data[index + 2] = paletteColor[2];
                        quantizedImage.data[index + 3] = 255;
                        quantizedImage.colorIndexes[bmpIndex] =
                            closestPaletteIndex * colorsPerPalette +
                                closestColorIndex +
                                adjustedIndex;
                    }
                }
            }
        }
    }
    return quantizedImage;
    function addBmpColors(palettes, bmpPalette) {
        let i = 0;
        for (const pal of palettes) {
            if (adjustedIndex === 1) {
                bmpPalette[i] = colorZero[2];
                bmpPalette[i + 1] = colorZero[1];
                bmpPalette[i + 2] = colorZero[0];
                i += 4;
            }
            for (const color of pal) {
                bmpPalette[i] = color[2];
                bmpPalette[i + 1] = color[1];
                bmpPalette[i + 2] = color[0];
                i += 4;
            }
        }
    }
}
function colorQuantize1Color(tiles, pixels, randomShuffle) {
    let iterations = quantizationOptions.fractionOfPixels * pixels.length;
    let alpha = 0.3;
    if (quantizationOptions.dither === Dither.Slow) {
        iterations /= 5;
        alpha = 0.1;
    }
    const avgColor = [0, 0, 0];
    for (const pixel of pixels) {
        addColor(avgColor, pixel.color);
    }
    scaleColor(avgColor, 1.0 / pixels.length);
    const palettes = [[avgColor]];
    if (quantizationOptions.colorZeroBehaviour === ColorZeroBehaviour.Shared) {
        palettes[0].push(avgColor);
        palettes[0][0] = structuredClone(quantizationOptions.colorZeroValue);
    }
    let splitIndex = 0;
    for (let numPalettes = 2; numPalettes <= quantizationOptions.numPalettes; numPalettes++) {
        palettes.push(structuredClone(palettes[splitIndex]));
        for (let iteration = 0; iteration < iterations; iteration++) {
            const nextPixel = pixels[randomShuffle.next()];
            movePalettesCloser(palettes, nextPixel, alpha);
        }
        const paletteDistance = zeroArray(numPalettes);
        for (const tile of tiles) {
            const [palIndex, distance] = closestPaletteDistance(palettes, tile);
            paletteDistance[palIndex] += distance;
        }
        splitIndex = maxIndex(paletteDistance);
    }
    return palettes;
}
function expandPalettesByOneColor(palettes, tiles, pixels, randomShuffle) {
    let iterations = quantizationOptions.fractionOfPixels * pixels.length;
    let alpha = 0.3;
    if (quantizationOptions.dither === Dither.Slow) {
        iterations /= 5;
        alpha = 0.1;
    }
    const numColors = palettes[0].length + 1;
    const splitIndexes = zeroArray(palettes.length);
    if (numColors > 2) {
        const totalColorDistances = [];
        for (let i = 0; i < palettes.length; i++) {
            const totalColorDistance = zeroArray(numColors);
            totalColorDistances.push(totalColorDistance);
        }
        for (const tile of tiles) {
            const closestPaletteIndex = getClosestPaletteIndex(palettes, tile);
            const palette = palettes[closestPaletteIndex];
            const colors = tile.colors;
            const counts = tile.counts;
            for (let i = 0; i < colors.length; i++) {
                const [minIndex, minDist] = getClosestColor(palette, colors[i]);
                totalColorDistances[closestPaletteIndex][minIndex] +=
                    counts[i] * minDist;
            }
        }
        for (let i = 0; i < palettes.length; i++) {
            splitIndexes[i] = maxIndex(totalColorDistances[i]);
        }
    }
    for (let i = 0; i < palettes.length; i++) {
        const colors = palettes[i];
        const splitIndex = splitIndexes[i];
        colors.push(cloneColor(colors[splitIndex]));
    }
    for (let iteration = 0; iteration < iterations; iteration++) {
        const nextPixel = pixels[randomShuffle.next()];
        movePalettesCloser(palettes, nextPixel, alpha);
    }
}
function colorQuantize1Palette(pixels, randomShuffle, colorsPerPalette) {
    let iterations = quantizationOptions.fractionOfPixels * pixels.length;
    if (quantizationOptions.dither === Dither.Slow) {
        iterations /= 5;
    }
    const errorStartIteration = iterations * 0.5;
    const alpha = 0.3;
    const colorZeroBehaviour = quantizationOptions.colorZeroBehaviour;
    if (colorZeroBehaviour === ColorZeroBehaviour.TransparentFromColor ||
        colorZeroBehaviour === ColorZeroBehaviour.TransparentFromTransparent) {
        colorsPerPalette -= 1;
    }
    // find average color
    const avgColor = [0, 0, 0];
    for (const pixel of pixels) {
        addColor(avgColor, pixel.color);
    }
    scaleColor(avgColor, 1.0 / pixels.length);
    let sharedColorIndex = -1;
    if (colorZeroBehaviour === ColorZeroBehaviour.Shared) {
        sharedColorIndex = 0;
    }
    const colors = [avgColor];
    let splitIndex = 0;
    for (let numColors = 2; numColors <= colorsPerPalette; numColors++) {
        if (numColors === 2 &&
            colorZeroBehaviour === ColorZeroBehaviour.Shared) {
            colors[0] = cloneColor(quantizationOptions.colorZeroValue);
            colors.push(avgColor);
        }
        else {
            colors.push(cloneColor(colors[splitIndex]));
        }
        const totalColorDistance = new Array(numColors);
        for (let i = 0; i < numColors; i++) {
            totalColorDistance[i] = 0.0;
        }
        for (let iteration = 0; iteration < iterations; iteration++) {
            const nextPixel = pixels[randomShuffle.next()];
            let minColorIndex = -1;
            let minColorDistance = -1;
            let targetColor;
            if (quantizationOptions.dither === Dither.Slow) {
                [minColorIndex, minColorDistance, targetColor] =
                    getClosestColorDither(colors, nextPixel);
            }
            else {
                [minColorIndex, minColorDistance] = getClosestColor(colors, nextPixel.color);
                targetColor = nextPixel.color;
            }
            if (minColorIndex !== sharedColorIndex) {
                moveColorCloser(colors[minColorIndex], targetColor, alpha);
            }
            if (iteration > errorStartIteration) {
                totalColorDistance[minColorIndex] += minColorDistance;
            }
        }
        splitIndex = maxIndex(totalColorDistance);
    }
    return colors;
}
function cloneColor(color) {
    const result = [0, 0, 0];
    for (let i = 0; i < 3; i++) {
        result[i] = color[i];
    }
    return result;
}
function copyColor(dest, source) {
    for (let i = 0; i < 3; i++) {
        dest[i] = source[i];
    }
}
function addColor(c1, c2) {
    for (let i = 0; i < 3; i++) {
        c1[i] += c2[i];
    }
}
function subtractColor(c1, c2) {
    for (let i = 0; i < 3; i++) {
        c1[i] -= c2[i];
    }
}
function scaleColor(color, scaleFactor) {
    for (let i = 0; i < 3; i++) {
        color[i] *= scaleFactor;
    }
}
function clampColor(color, minValue, maxValue) {
    for (let i = 0; i < 3; i++) {
        if (color[i] < minValue) {
            color[i] = minValue;
        }
        else if (color[i] > maxValue) {
            color[i] = maxValue;
        }
    }
}
// alpha = 255 / (2 ** n - 1)
const alphaValues = [0, 255, 85, 36.42857, 17, 8.22581, 4.04762, 2.00787, 1];
function toNbit(value, n) {
    const alpha = alphaValues[n];
    return Math.round(Math.round(value / alpha) * alpha);
}
function toNbitColor(color, n) {
    for (let i = 0; i < 3; i++) {
        color[i] = toNbit(color[i], n);
    }
}
function moveColorCloser(color, pixelColor, alpha) {
    for (let i = 0; i < color.length; i++) {
        color[i] = (1 - alpha) * color[i] + alpha * pixelColor[i];
    }
}
function maxIndex(values) {
    let maxI = 0;
    for (let i = 1; i < values.length; i++) {
        if (values[i] > values[maxI]) {
            maxI = i;
        }
    }
    return maxI;
}
function minIndex(values) {
    let minI = 0;
    for (let i = 1; i < values.length; i++) {
        if (values[i] < values[minI]) {
            minI = i;
        }
    }
    return minI;
}
//# sourceMappingURL=worker.js.map