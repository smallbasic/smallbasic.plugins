REM QR Code generator library (C)
REM
REM Copyright (c) Project Nayuki. (MIT License)
REM https://www.nayuki.io/page/qr-code-generator-library
REM
REM Permission is hereby granted, free of charge, to any person obtaining a copy of
REM this software and associated documentation files (the Software), to deal in
REM the Software without restriction, including without limitation the rights to
REM use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
REM the Software, and to permit persons to whom the Software is furnished to do so,
REM subject to the following conditions:
REM - The above copyright notice and this permission notice shall be included in
REM   all copies or substantial portions of the Software.
REM - The Software is provided as is, without warranty of any kind, express or
REM   implied, including but not limited to the warranties of merchantability,
REM   fitness for a particular purpose and noninfringement. In no event shall the
REM   authors or copyright holders be liable for any claim, damages or other
REM   liability, whether in an action of contract, tort or otherwise, arising from,
REM   out of or in connection with the Software or the use or other dealings in the
REM   Software.

unit qrcode

export qrcodegen_encodeText
export qrcodegen_BUFFER_LEN_FOR_VERSION
export qrcodegen_getSize
export qrcodegen_getModule

' The set of all legal characters in alphanumeric mode, where each character
' value maps to the index in the string. For checking text and encoding segments.
ALPHANUMERIC_CHARSET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:"

' For generating error correction codes.
ECC_CODEWORDS_PER_BLOCK = [
  ' Version: (note that index 0 is for padding, and is set to an illegal value)
  '0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
  [-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30],  ' Low
  [-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28],  ' Medium
  [-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30],  ' Quartile
  [-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30],  ' High
]

const qrcodegen_REED_SOLOMON_DEGREE_MAX = 30  ' Based on the table above

' For generating error correction codes.
NUM_ERROR_CORRECTION_BLOCKS = [
  ' Version: (note that index 0 is for padding, and is set to an illegal value)
  '0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
  [-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25],  ' Low
  [-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49],  ' Medium
  [-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68],  ' Quartile
  [-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81],  ' High
]

export const qrcodegen_Ecc_LOW = 0
export const qrcodegen_Ecc_MEDIUM  = 1
export const qrcodegen_Ecc_QUARTILE = 2
export const qrcodegen_Ecc_HIGH = 3
export const qrcodegen_Mask_AUTO = -1
export const qrcodegen_Mode_NUMERIC      = 0x1
export const qrcodegen_Mode_ALPHANUMERIC = 0x2
export const qrcodegen_Mode_BYTE         = 0x4
export const qrcodegen_Mode_KANJI        = 0x8
export const qrcodegen_Mode_ECI          = 0x7
export const qrcodegen_VERSION_MIN = 1
export const qrcodegen_VERSION_MAX = 40

' For automatic mask pattern selection.
const PENALTY_N1 =  3
const PENALTY_N2 =  3
const PENALTY_N3 = 40
const PENALTY_N4 = 10
const qrcodegen_Mask_0 = 0
const qrcodegen_Mask_1 = 1
const qrcodegen_Mask_2 = 2
const qrcodegen_Mask_3 = 3
const qrcodegen_Mask_4 = 4
const qrcodegen_Mask_5 = 5
const qrcodegen_Mask_6 = 6
const qrcodegen_Mask_7 = 7

const INT16_MAX = maxint / 2

sub assert(t, l)
  if (!t) then throw "Assertion failed at line: " + l
end

def qrcodegen_BUFFER_LEN_FOR_VERSION(n) = ((((n) * 4 + 17) * ((n) * 4 + 17) + 7) / 8 + 1)

sub memset(byref a, v, n)
  assert(isarray(a), PROGLINE)
  local i
  for i = 0 to n - 1
    a[i] = v
  next i
end

REM
REM copies n bytes from memory area src to memory area dest. The memory areas may overlap: copying takes place as
REM though the bytes in src are first copied into a temporary array that does not overlap src or dest, and the
REM bytes are then copied from the temporary array to dest.
REM
sub memmove(byref a, dest, src, n)
  assert(isarray(a), PROGLINE)
  local i, j, tmp
  dim tmp(n)

  for i = 0 to n - 1
    tmp[i] = a[i + src]
  next i
  for i = 0 to n - 1
    a[i + dest] = tmp[i]
  next i
end

' The worst-case number of bytes needed to store one QR Code, up to and including
' version 40. This value equals 3918, which is just under 4 kilobytes.
' Use this more convenient value to avoid calculating tighter memory bounds for buffers.
const qrcodegen_BUFFER_LEN_MAX  = qrcodegen_BUFFER_LEN_FOR_VERSION(qrcodegen_VERSION_MAX)

'---- High-level QR Code encoding functions ----

REM
REM Encodes the given text string to a QR Code, returning true if encoding succeeded.
REM If the data is too long to fit in any version in the given range
REM at the given ECC level, then false is returned.
REM - The input text must be encoded in UTF-8 and contain no NULs.
REM - The variables ecl and mask must correspond to enum constant values.
REM - Requires 1 <= minVersion <= maxVersion <= 40.
REM - The arrays tempBuffer and qrcode must each have a length
REM   of at least qrcodegen_BUFFER_LEN_FOR_VERSION(maxVersion).
REM - After the function returns, tempBuffer contains no useful data.
REM - If successful, the resulting QR Code may use numeric,
REM   alphanumeric, or byte mode to encode the text.
REM - In the most optimistic case, a QR Code at version 40 with low ECC
REM   can hold any UTF-8 string up to 2953 bytes, or any alphanumeric string
REM   up to 4296 characters, or any digit string up to 7089 characters.
REM   These numbers represent the hard upper limit of the QR Code standard.
REM - Please consult the QR Code specification for information on
REM   data capacities per version, ECC level, and text encoding mode.
REM
func qrcodegen_encodeText(text, byref tempBuffer, byref qrcode, ecl, minVersion, maxVersion, mask, boostEcl)
  local textLen = len(text)
  if (textLen == 0) then return qrcodegen_encodeSegmentsAdvanced(0, 0, ecl, minVersion, maxVersion, mask, boostEcl, tempBuffer, qrcode)
  local bufLen = qrcodegen_BUFFER_LEN_FOR_VERSION(maxVersion)
  local seg = {}
  if (qrcodegen_isNumeric(text))
    if (qrcodegen_calcSegmentBufferSize(qrcodegen_Mode_NUMERIC, textLen) > bufLen) then throw "fail"
    seg = qrcodegen_makeNumeric(text, tempBuffer)
  else if (qrcodegen_isAlphanumeric(text))
    if (qrcodegen_calcSegmentBufferSize(qrcodegen_Mode_ALPHANUMERIC, textLen) > bufLen) then throw "fail"
    seg = qrcodegen_makeAlphanumeric(text, tempBuffer)
  else
    if (textLen > bufLen) then throw "fail"
    for i = 0 to textLen - 1
      tempBuffer[i] = asc(mid(text, i + 1, 1))
    next i
    seg.mode = qrcodegen_Mode_BYTE
    seg.bitLength = calcSegmentBitLength(seg.mode, textLen)
    if (seg.bitLength == -1) then throw "fail"
    seg.numChars = textLen
    seg._data = tempBuffer
  endif
  dim segs(1)
  segs[0] = seg
  return qrcodegen_encodeSegmentsAdvanced(segs, 1, ecl, minVersion, maxVersion, mask, boostEcl, tempBuffer, qrcode)
end

REM
REM Encodes the given binary data to a QR Code, returning true if encoding succeeded.
REM If the data is too long to fit in any version in the given range
REM at the given ECC level, then false is returned.
REM - The input array range dataAndTemp[0 : dataLen] should normally be
REM   valid UTF-8 text, but is not required by the QR Code standard.
REM - The variables ecl and mask must correspond to enum constant values.
REM - Requires 1 <= minVersion <= maxVersion <= 40.
REM - The arrays dataAndTemp and qrcode must each have a length
REM   of at least qrcodegen_BUFFER_LEN_FOR_VERSION(maxVersion).
REM - After the function returns, the contents of dataAndTemp may have changed,
REM   and does not represent useful data anymore.
REM - If successful, the resulting QR Code will use byte mode to encode the data.
REM - In the most optimistic case, a QR Code at version 40 with low ECC can hold any byte
REM   sequence up to length 2953. This is the hard upper limit of the QR Code standard.
REM - Please consult the QR Code specification for information on
REM   data capacities per version, ECC level, and text encoding mode.
REM
func qrcodegen_encodeBinary(byref dataAndTemp, dataLen, byref qrcode, byref ecl, minVersion, maxVersion, mask, boostEcl)
  dim segs(1)
  local seg = {}
  seg.mode = qrcodegen_Mode_BYTE
  seg.bitLength = calcSegmentBitLength(seg.mode, dataLen)
  if (seg.bitLength == -1)
    qrcode[0] = 0  ' Set size to invalid value for safety
    return false
  endif
  seg.numChars = dataLen
  seg._data = dataAndTemp
  segs[0] = seg
  return qrcodegen_encodeSegmentsAdvanced(seg, 1, ecl, minVersion, maxVersion, mask, boostEcl, dataAndTemp, qrcode)
end

REM
REM Appends the given number of low-order bits of the given value to the given byte-based
REM bit buffer, increasing the bit length. Requires 0 <= numBits <= 16 and val < 2^numBits.
REM
func appendBitsToBuffer(value, numBits, byref buffer, bitLen)
  assert(0 <= numBits && numBits <= 16 && (value == 0 || (value rshift numBits == 0)), PROGLINE)
  local i, idx
  for i = numBits - 1 to 0 step -1
    idx = bitLen rshift 3
    buffer[idx] = buffer[idx] | (((value rshift i) & 1) lshift (7 - (bitLen & 7)))
    bitLen++
  next i
  return bitLen
end

'---- Low-level QR Code encoding functions ----

REM
REM Renders a QR Code representing the given segments at the given error correction level.
REM The smallest possible QR Code version is automatically chosen for the output. Returns true if
REM QR Code creation succeeded, or false if the data is too long to fit in any version. The ECC level
REM of the result may be higher than the ecl argument if it can be done without increasing the version.
REM This function allows the user to create a custom sequence of segments that switches
REM between modes (such as alphanumeric and byte) to encode text in less space.
REM This is a low-level API; the high-level API is qrcodegen_encodeText() and qrcodegen_encodeBinary().
REM To save memory, the segments data buffers can alias/overlap tempBuffer, and will
REM result in them being clobbered, but the QR Code output will still be correct.
REM But the qrcode array must not overlap tempBuffer or any segments data buffer.
REM
func qrcodegen_encodeSegments(byref segs, lenSegs, ecl, byref tempBuffer, byref qrcode)
  return qrcodegen_encodeSegmentsAdvanced(segs, lenSegs, ecl, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true, tempBuffer, qrcode)
end

REM
REM Renders a QR Code representing the given segments with the given encoding parameters.
REM Returns true if QR Code creation succeeded, or false if the data is too long to fit in the range of versions.
REM The smallest possible QR Code version within the given range is automatically
REM chosen for the output. Iff boostEcl is true, then the ECC level of the result
REM may be higher than the ecl argument if it can be done without increasing the
REM version. The mask is either between qrcodegen_Mask_0 to 7 to force that mask, or
REM qrcodegen_Mask_AUTO to automatically choose an appropriate mask (which may be slow).
REM This function allows the user to create a custom sequence of segments that switches
REM between modes (such as alphanumeric and byte) to encode text in less space.
REM This is a low-level API; the high-level API is qrcodegen_encodeText() and qrcodegen_encodeBinary().
REM To save memory, the segments data buffers can alias/overlap tempBuffer, and will
REM result in them being clobbered, but the QR Code output will still be correct.
REM But the qrcode array must not overlap tempBuffer or any segments data buffer.
REM
func qrcodegen_encodeSegmentsAdvanced(byref segs, lenSegs, ecl, minVersion, maxVersion, mask, boostEcl, byref tempBuffer, byref qrcode)
  assert(isarray(segs) && lenSegs > 0, PROGLINE)
  assert(qrcodegen_VERSION_MIN <= minVersion && minVersion <= maxVersion && maxVersion <= qrcodegen_VERSION_MAX, PROGLINE)
  assert(0 <= ecl && ecl <= 3 && -1 <= mask && mask <= 7, PROGLINE)

  ' Find the minimal version number to use
  local version = minVersion
  local found = false
  local dataUsedBits

  while !found
    local dataCapacityBits = getNumDataCodewords(version, ecl) * 8  ' Number of data bits available
    dataUsedBits = getTotalBits(segs, lenSegs, version)
    if (dataUsedBits != -1 && dataUsedBits <= dataCapacityBits) then
      ' This version number is found to be suitable
      found = true
    else
      if (version >= maxVersion) then
        ' All versions in the range could not fit the given data
        qrcode[0] = 0  ' Set size to invalid value for safety
        return false
      endif
      version++
    endif
  wend
  assert(dataUsedBits != -1, PROGLINE)

  ' Increase the error correction level while the data still fits in the current version number
  local i
  for i = qrcodegen_Ecc_MEDIUM to qrcodegen_Ecc_HIGH  ' From low to high
    if (boostEcl && dataUsedBits <= getNumDataCodewords(version, i) * 8) then
      ecl = i
    endif
  next i

  ' Concatenate all segments to create the data bit string
  local bitLen = 0
  local bit, j
  for i = 0 to lenSegs - 1
    bitLen = appendBitsToBuffer(segs[i].mode, 4, qrcode, bitLen)
    bitLen = appendBitsToBuffer(segs[i].numChars, numCharCountBits(segs[i].mode, version), qrcode, bitLen)
    for j = 0 to segs[i].bitLength - 1
      bit = (segs[i]._data[j rshift 3] rshift (7 - (j & 7))) & 1
      bitLen = appendBitsToBuffer(bit, 1, qrcode, bitLen)
    next j
  next i

  assert(bitLen == dataUsedBits, PROGLINE)

  ' Add terminator and pad up to a byte if applicable
  local dataCapacityBits = getNumDataCodewords(version, ecl) * 8
  assert(bitLen <= dataCapacityBits, PROGLINE)
  local terminatorBits = dataCapacityBits - bitLen
  if (terminatorBits > 4) then terminatorBits = 4
  bitLen = appendBitsToBuffer(0, terminatorBits, qrcode, bitLen)
  bitLen = appendBitsToBuffer(0, (8 - bitLen % 8) % 8, qrcode, bitLen)
  assert(bitLen % 8 == 0, PROGLINE)

  ' Pad with alternating bytes until data capacity is reached
  local padByte = 0xEC
  while bitLen < dataCapacityBits
    bitLen = appendBitsToBuffer(padByte, 8, qrcode, bitLen)
    padByte = (padByte xor 0xEC) xor 0x11
  wend


  ' Draw function and data codeword modules
  addEccAndInterleave(qrcode, version, ecl, tempBuffer)

  initializeFunctionModules(version, qrcode)
  drawCodewords(tempBuffer, getNumRawDataModules(version) / 8, qrcode)
  drawWhiteFunctionModules(qrcode, version)
  initializeFunctionModules(version, tempBuffer)

  ' Handle masking
  if (mask == qrcodegen_Mask_AUTO) then
    ' Automatically choose best mask
    local minPenalty = maxint
    for i = 0 to 7
      local msk = i
      applyMask(tempBuffer, qrcode, msk)
      drawFormatBits(ecl, msk, qrcode)
      local penalty = getPenaltyScore(qrcode)
      if (penalty < minPenalty) then
        mask = msk
        minPenalty = penalty
      endif
      applyMask(tempBuffer, qrcode, msk)  ' Undoes the mask due to XOR
    next i
  endif

  assert(0 <= mask && mask <= 7, PROGLINE)
  applyMask(tempBuffer, qrcode, mask)
  drawFormatBits(ecl, mask, qrcode)
  return true
end

'---- Error correction code generation functions ----

REM
REM Appends error correction bytes to each block of the given data array, then interleaves
REM bytes from the blocks and stores them in the result array. data[0 : dataLen] contains
REM the input data. data[dataLen : rawCodewords] is used as a temporary work area and will
REM be clobbered by this function. The final answer is stored in result[0 : rawCodewords].
REM
sub addEccAndInterleave(byref segData, version, ecl, byref result)
  ' Calculate parameter numbers
  assert(0 <= ecl && ecl < 4 && qrcodegen_VERSION_MIN <= version && version <= qrcodegen_VERSION_MAX, PROGLINE)

  local numBlocks = NUM_ERROR_CORRECTION_BLOCKS[ecl][version]
  local blockEccLen = ECC_CODEWORDS_PER_BLOCK  [ecl][version]
  local rawCodewords = getNumRawDataModules(version) / 8
  local dataLen = getNumDataCodewords(version, ecl)
  local numShortBlocks = numBlocks - rawCodewords % numBlocks
  local shortBlockDataLen = rawCodewords / numBlocks - blockEccLen

  ' Split data into blocks, calculate ECC, and interleave
  ' (not concatenate) the bytes into a single sequence
  dim rsdiv(qrcodegen_REED_SOLOMON_DEGREE_MAX)
  reedSolomonComputeDivisor(blockEccLen, rsdiv)

  local i, j, k, datIndex, jdataLen, ecc
  datIndex = 0
  for i = 0 to numBlocks - 1
    datLen = shortBlockDataLen + iff(i < numShortBlocks, 0, 1)
    dim ecc(len(segData))  ' Temporary storage
    reedSolomonComputeRemainder(segData, datIndex, datLen, rsdiv, blockEccLen, ecc)

    ' Copy data
    k = i
    for j = 0 to datLen - 1
      if (j == shortBlockDataLen) then
        k -= numShortBlocks
      endif
      result[k] = segData[datIndex + j]
      k += numBlocks
    next j

    ' Copy ECC
    k = dataLen + i
    for j = 0 to blockEccLen -1
      result[k] = ecc[j]
      k += numBlocks
    next j
    datIndex += datLen
  next i
end

REM
REM Returns the number of 8-bit codewords that can be used for storing data (not ECC),
REM for the given version number and error correction level. The result is in the range [9, 2956].
REM
func getNumDataCodewords(version, ecl)
  local v = version
  local e = ecl
  assert(0 <= e && e < 4, PROGLINE)
  return getNumRawDataModules(v) / 8 - ECC_CODEWORDS_PER_BLOCK[e][v] * NUM_ERROR_CORRECTION_BLOCKS[e][v]
end

REM
REM Returns the number of data bits that can be stored in a QR Code of the given version number, after
REM all function modules are excluded. This includes remainder bits, so it might not be a multiple of 8.
REM The result is in the range [208, 29648]. This could be implemented as a 40-entry lookup table.
REM
func getNumRawDataModules(ver)
  assert(qrcodegen_VERSION_MIN <= ver && ver <= qrcodegen_VERSION_MAX, PROGLINE)
  local result = (16 * ver + 128) * ver + 64
  if (ver >= 2)
    local numAlign = int(ver / 7) + 2
    result -= int((25 * numAlign - 10) * numAlign - 55)
    if (ver >= 7) then result -= 36
  endif
  assert(208 <= result && result <= 29648, PROGLINE)
  return result
end

'---- Reed-Solomon ECC generator functions ----

REM
REM Computes a Reed-Solomon ECC generator polynomial for the given degree, storing in result[0 : degree].
REM This could be implemented as a lookup table over all possible parameter values, instead of as an algorithm.
REM
sub reedSolomonComputeDivisor(degree, byref result)
  assert(1 <= degree && degree <= qrcodegen_REED_SOLOMON_DEGREE_MAX, PROGLINE)
  ' Polynomial coefficients are stored from highest to lowest power, excluding the leading term which is always 1.
  ' For example the polynomial x^3 + 255x^2 + 8x + 93 is stored as the uint8 array {255, 8, 93}.
  memset(result, 0, degree)
  result[degree - 1] = 1  ' Start off with the monomial x^0

  ' Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
  ' drop the highest monomial term which is always 1x^degree.
  ' Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
  local rootx = 1
  local i, j

  for i = 0 to degree - 1
    ' Multiply the current product by (x - r^i)
    for j = 0 to degree - 1
      result[j] = reedSolomonMultiply(result[j], rootx)
      if (j + 1 < degree) then
        result[j] = result[j] xor result[j + 1]
      endif
    next j
    rootx = reedSolomonMultiply(rootx, 0x02)
  next i
end

REM
REM Computes the Reed-Solomon error correction codeword for the given data and divisor polynomials.
REM The remainder when data[0 : dataLen] is divided by divisor[0 : degree] is stored in result[0 : degree].
REM All polynomials are in big endian, and the generator has an implicit leading 1 term.
REM
sub reedSolomonComputeRemainder(byref dataSeg, datIndex, dataLen, byref generator, degree, byref result)
  assert(1 <= degree && degree <= qrcodegen_REED_SOLOMON_DEGREE_MAX, PROGLINE)
  memset(result, 0, degree)
  local i, j
  for i = 0 to dataLen - 1  ' Polynomial division
    factor = dataSeg[datIndex + i] xor result[datIndex]
    memmove(result, 0, 1, degree - 1)
    result[degree - 1] = 0
    for j = 0 to degree - 1
      result[j] = result[j] xor reedSolomonMultiply(generator[j], factor)
    next j
  next i
end

REM
REM Returns the product of the two given field elements modulo GF(2^8/0x11D).
REM All inputs are valid. This could be implemented as a 256*256 lookup table.
REM
func reedSolomonMultiply(x, y)
  ' Russian peasant multiplication
  local i, z

  z = 0
  for i = 7 to 0 step - 1
    z = z lshift 1 xor ((z rshift 7) * 0x11D)
    z = z xor ((y rshift i) band 1) * x
  next i
  return z
end

'---- Drawing function modules ----

REM
REM Clears the given QR Code grid with white modules for the given
REM versions size, then marks every function module as black.
REM
sub initializeFunctionModules(version, byref qrcode)
  ' Initialize QR Code
  local qrsize = version * 4 + 17
  memset(qrcode, 0, ((qrsize * qrsize + 7) / 8 + 1))
  qrcode[0] = qrsize

  ' Fill horizontal and vertical timing patterns
  fillRectangle(6, 0, 1, qrsize, qrcode)
  fillRectangle(0, 6, qrsize, 1, qrcode)

  ' Fill 3 finder patterns (all corners except bottom right) and format bits
  fillRectangle(0, 0, 9, 9, qrcode)
  fillRectangle(qrsize - 8, 0, 8, 9, qrcode)
  fillRectangle(0, qrsize - 8, 9, 8, qrcode)

  ' Fill numerous alignment patterns
  dim alignPatPos(7)
  local numAlign = getAlignmentPatternPositions(version, alignPatPos)
  local i, j

  for i = 0 to numAlign - 1
    for j = 0 to numAlign - 1
      ' Don't draw on the three finder corners
      if (!((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0))) then
        fillRectangle(alignPatPos[i] - 2, alignPatPos[j] - 2, 5, 5, qrcode)
      endif
    next j
  next i

  ' Fill version blocks
  if (version >= 7) then
    fillRectangle(qrsize - 11, 0, 3, 6, qrcode)
    fillRectangle(0, qrsize - 11, 6, 3, qrcode)
  endif

end

REM
REM Draws white function modules and possibly some black modules onto the given QR Code, without changing
REM non-function modules. This does not draw the format bits. This requires all function modules to be previously
REM marked black (namely by initializeFunctionModules()), because this may skip redrawing black function modules.
REM
sub drawWhiteFunctionModules(byref qrcode, version)
  ' Draw horizontal and vertical timing patterns
  local qrsize = qrcodegen_getSize(qrcode)
  local i, j, dx, dy, dist, alignPatPos

  for i = 7 to (qrsize - 7) step 2
    setModule(qrcode, 6, i, false)
    setModule(qrcode, i, 6, false)
  next i

  ' Draw 3 finder patterns (all corners except bottom right overwrites some timing modules)
  for dy = -4 to 4
    for dx = -4 to 4
      dist = abs(dx)
      if (abs(dy) > dist) then dist = abs(dy)
      if (dist == 2 || dist == 4) then
        setModuleBounded(qrcode, 3 + dx, 3 + dy, false)
        setModuleBounded(qrcode, qrsize - 4 + dx, 3 + dy, false)
        setModuleBounded(qrcode, 3 + dx, qrsize - 4 + dy, false)
      endif
    next dx
  next dy

  ' Draw numerous alignment patterns
  dim alignPatPos(7)
  local numAlign = getAlignmentPatternPositions(version, alignPatPos)
  for i = 0 to numAlign - 1
    for j = 0 to numAlign - 1
      if ((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0)) then
        'continue  ' Don't draw on the three finder corners
      else
        for dy = -1 to 1
          for dx = -1 to 1
            setModule(qrcode, alignPatPos[i] + dx, alignPatPos[j] + dy, (dx == 0 && dy == 0))
          next dx
        next dy
      endif
    next j
  next i

  ' Draw version blocks
  if (version >= 7) then
    ' Calculate error correction code and pack bits
    local vrem = version  ' version is uint6, in the range [7, 40]
    for i = 0 to 11
      vrem = (vrem lshift 1) xor ((vrem rshift 11) * 0x1F25)
    next i
    local bits = version lshift 12 | vrem  ' uint18
    assert(bits rshift 18 == 0, PROGLINE)

    ' Draw two copies
    for i = 0 to 5
      for j = 0 to 2
        local k = qrsize - 11 + j
        setModule(qrcode, k, i, (bits & 1) != 0)
        setModule(qrcode, i, k, (bits & 1) != 0)
        bits = bits rshift 1
      next j
    next i
  endif
end

REM
REM Draws two copies of the format bits (with its own error correction code) based
REM on the given mask and error correction level. This always draws all modules of
REM the format bits, unlike drawWhiteFunctionModules() which might skip black modules.
REM
sub drawFormatBits(ecl, mask, byref qrcode)
  ' Calculate error correction code and pack bits
  assert(0 <= mask && mask <= 7, PROGLINE)
  table = [1, 0, 3, 2]
  local qdata = table[ecl] lshift 3 | mask  ' errCorrLvl is uint2, mask is uint3
  local qrem = qdata
  local i

  for i = 0 to 9
    qrem = (qrem lshift 1) xor ((qrem rshift 9) * 0x537)
  next i
  local bits = (qdata lshift 10 | qrem) xor 0x5412  ' uint15
  assert(bits rshift 15 == 0, PROGLINE)

  ' Draw first copy
  for i = 0 to 5
    setModule(qrcode, 8, i, getBit(bits, i))
  next i
  setModule(qrcode, 8, 7, getBit(bits, 6))
  setModule(qrcode, 8, 8, getBit(bits, 7))
  setModule(qrcode, 7, 8, getBit(bits, 8))
  for i = 9 to 14
    setModule(qrcode, 14 - i, 8, getBit(bits, i))
  next i
  ' Draw second copy
  local qrsize = qrcodegen_getSize(qrcode)
  for i = 0 to 7
    setModule(qrcode, qrsize - 1 - i, 8, getBit(bits, i))
  next i
  for i = 8 to 14
    setModule(qrcode, 8, qrsize - 15 + i, getBit(bits, i))
  next i
  setModule(qrcode, 8, qrsize - 8, true)  ' Always black
end

REM
REM Calculates and stores an ascending list of positions of alignment patterns
REM for this version number, returning the length of the list (in the range [0,7]).
REM Each position is in the range [0,177), and are used on both the x and y axes.
REM This could be implemented as lookup table of 40 variable-length lists of unsigned bytes.
REM
func getAlignmentPatternPositions(version, byref result)
  if (version == 1) then
    return 0
  endif

  local numAlign = int(version / 7) + 2
  local qstep = iff(version == 32, 26, (version * 4 + numAlign * 2 + 1) / (numAlign * 2 - 2) * 2)
  local pos = version * 4 + 10
  local i

  for i = numAlign - 1 to 0 step -1
    result[i] = pos
    pos -= qstep
  next i
  result[0] = 6
  return numAlign
end

REM
REM Sets every pixel in the range [left : left + width] * [top : top + height] to black.
REM
sub fillRectangle(_left, _top, _width, _height, byref qrcode)
  local dx, dy

  for dy = 0 to _height - 1
    for dx = 0 to _width - 1
      setModule(qrcode, _left + dx, _top + dy, true)
    next dx
  next dy
end

'---- Drawing data modules and masking ----

REM
REM Draws the raw codewords (including data and ECC) onto the given QR Code. This requires the initial state of
REM the QR Code to be black at function modules and white at codeword modules (including unused remainder bits).
REM
sub drawCodewords(byref _data, dataLen, byref qrcode)
  local qrsize = qrcodegen_getSize(qrcode)
  local i = 0  ' Bit index into the data
  local j, x, y, xright, black, vert, upward

  ' Do the funny zigzag scan
  for xright = qrsize - 1 to 1 step -2  ' Index of right column in each column pair
    if (xright == 6) then xright = 5
    for vert = 0 to qrsize -1  ' Vertical counter
      for j = 0 to 1
        x = xright - j  ' Actual x coordinate
        upward = ((xright + 1) & 2) == 0
        y = iff(upward, qrsize - 1 - vert, vert)  ' Actual y coordinate
        if (!getModule(qrcode, x, y) && i < (dataLen * 8)) then
          black = getBit(_data[i rshift 3], 7 - (i & 7))
          setModule(qrcode, x, y, black)
          i++
        endif
        ' If this QR Code has any remainder bits (0 to 7), they were assigned as
        ' 0/false/white by the constructor and are left unchanged by this method
      next j
    next vert
  next xright
  assert(i == dataLen * 8, PROGLINE)
end

REM
REM XORs the codeword modules in this QR Code with the given mask pattern.
REM The function modules must be marked and the codeword bits must be drawn
REM before masking. Due to the arithmetic of XOR, calling applyMask() with
REM the same mask value a second time will undo the mask. A final well-formed
REM QR Code needs exactly one (not zero, two, etc.) mask applied.
REM
sub applyMask(byref functionModules, byref qrcode, mask)
  assert(0 <= mask && mask <= 7, PROGLINEO)  ' Disallows qrcodegen_Mask_AUT
  local qrsize = qrcodegen_getSize(qrcode)
  local invert, x, y, _val

  for y = 0 to qrsize - 1
    for x = 0 to qrsize - 1
      if (!getModule(functionModules, x, y)) then
        select case mask
        case 0:  invert = (x + y) % 2 == 0
        case 1:  invert = y % 2 == 0
        case 2:  invert = x % 3 == 0
        case 3:  invert = (x + y) % 3 == 0
        case 4:  invert = (x / 3 + y / 2) % 2 == 0
        case 5:  invert = x * y % 2 + x * y % 3 == 0
        case 6:  invert = (x * y % 2 + x * y % 3) % 2 == 0
        case 7:  invert = ((x + y) % 2 + x * y % 3) % 2 == 0
        case else assert(false, PROGLINE)
        end select
        _val = getModule(qrcode, x, y)
        setModule(qrcode, x, y, _val xor invert)
      endif
    next x
  next y
end

REM
REM Calculates and returns the penalty score based on state of the given QR Codes current modules.
REM This is used by the automatic mask choice algorithm to find the mask pattern that yields the lowest score.
REM
func getPenaltyScore(byref qrcode)
  local qrsize = qrcodegen_getSize(qrcode)
  local result = 0
  local runHistory, x, y

  ' Adjacent modules in row having same color, and finder-like patterns
  for y = 0 to qrsize - 1
    local runColor = false
    local runX = 0
    dim runHistory(7)
    for x = 0 to qrsize - 1
      if (getModule(qrcode, x, y) == runColor)
        runX++
        if (runX == 5) then
          result += PENALTY_N1
        else if (runX > 5)
          result++
        endif
      else
        finderPenaltyAddHistory(runX, runHistory, qrsize)
        if (!runColor) then result += finderPenaltyCountPatterns(runHistory, qrsize) * PENALTY_N3
        runColor = getModule(qrcode, x, y)
        runX = 1
      endif
    next x
    result += finderPenaltyTerminateAndCount(runColor, runX, runHistory, qrsize) * PENALTY_N3
  next y

  ' Adjacent modules in column having same color, and finder-like patterns
  for x = 0 to qrsize - 1
    local runColor = false
    local runY = 0
    dim runHistory(7)
    for y = 0 to qrsize - 1
      if (getModule(qrcode, x, y) == runColor) then
        runY++
        if (runY == 5) then
          result += PENALTY_N1
        else if (runY > 5)
          result++
        endif
      else
        finderPenaltyAddHistory(runY, runHistory, qrsize)
        if (!runColor) then result += finderPenaltyCountPatterns(runHistory, qrsize) * PENALTY_N3
        runColor = getModule(qrcode, x, y)
        runY = 1
      endif
    next y
    result += finderPenaltyTerminateAndCount(runColor, runY, runHistory, qrsize) * PENALTY_N3
  next x

  ' 2*2 blocks of modules having same color
  for y = 0 to qrsize - 2
    for x = 0 to qrsize - 2
      _color = getModule(qrcode, x, y)
      if (_color == getModule(qrcode, x + 1, y) &&
          _color == getModule(qrcode, x, y + 1) &&
          _color == getModule(qrcode, x + 1, y + 1)) then
        result += PENALTY_N2
      endif
    next x
  next  y

  ' Balance of black and white modules
  local black = 0
  for y = 0 to qrsize - 1
    for x = 0 to qrsize - 1
      if (getModule(qrcode, x, y)) then  black++
    next x
  next y

  local total = qrsize * qrsize  ' Note that size is odd, so black/total != 1/2
  ' Compute the smallest integer k >= 0 such that (45-5k)% <= black/total <= (55+5k)%
  local k = ((abs(black * 20 - total * 10) + total - 1) / total) - 1
  result += k * PENALTY_N4
  return result
end

REM
REM Can only be called immediately after a white run is added, and
REM returns either 0, 1, or 2. A helper function for getPenaltyScore().
REM
func finderPenaltyCountPatterns(byref runHistory, qrsize)
  local n = runHistory[1]
  assert(n <= qrsize * 3, PROGLINE)
  local core = n > 0 && runHistory[2] == n && runHistory[3] == n * 3 && runHistory[4] == n && runHistory[5] == n
  ' The maximum QR Code size is 177, hence the black run length n <= 177.
  ' Arithmetic is promoted to int, so n*4 will not overflow.
  return iff(core && runHistory[0] >= n * 4 && runHistory[6] >= n, 1, 0) + iff(core && runHistory[6] >= n * 4 && runHistory[0] >= n, 1, 0)
end

REM
REM Must be called at the end of a line (row or column) of modules. A helper function for getPenaltyScore().
REM
func finderPenaltyTerminateAndCount(currentRunColor, currentRunLength, byref runHistory, qrsize)
  if (currentRunColor) then ' Terminate black run
    finderPenaltyAddHistory(currentRunLength, runHistory, qrsize)
    currentRunLength = 0
  endif
  currentRunLength += qrsize  ' Add white border to final run
  finderPenaltyAddHistory(currentRunLength, runHistory, qrsize)
  return finderPenaltyCountPatterns(runHistory, qrsize)
end

REM
REM Pushes the given value to the front and drops the last value. A helper function for getPenaltyScore().
REM
sub finderPenaltyAddHistory(currentRunLength, byref runHistory, qrsize)
  if (runHistory[0] == 0) then currentRunLength += qrsize  ' Add white border to initial run
  memmove(runHistory, 1, 0, 6)
  runHistory[0] = currentRunLength
end

'---- Basic QR Code information ----

REM
REM Returns the side length of the given QR Code, assuming that encoding succeeded.
REM The result is in the range [21, 177]. Note that the length of the array buffer
REM is related to the side length - every qrcode must have length at least
REM qrcodegen_BUFFER_LEN_FOR_VERSION(version), which equals ceil(size^2 / 8 + 1).
REM
func qrcodegen_getSize(byref qrcode)
  assert(isarray(qrcode), PROGLINE)
  local result = qrcode[0]
  assert((qrcodegen_VERSION_MIN * 4 + 17) <= result && result <= (qrcodegen_VERSION_MAX * 4 + 17), PROGLINE)
  return result
end

REM
REM Returns the color of the module (pixel) at the given coordinates, which is false
REM for white or true for black. The top left corner has the coordinates (x=0, y=0).
REM If the given coordinates are out of bounds, then false (white) is returned.
REM
func qrcodegen_getModule(byref qrcode, x, y)
  assert(isarray(qrcode), PROGLINE)
  local qrsize = qrcode[0]
  return (0 <= x && x < qrsize && 0 <= y && y < qrsize) && getModule(qrcode, x, y)
end

REM
REM Gets the module at the given coordinates, which must be in bounds.
REM
func getModule(byref qrcode, x, y)
  local qrsize = qrcode[0]
  assert(21 <= qrsize && qrsize <= 177 && 0 <= x && x < qrsize && 0 <= y && y < qrsize, PROGLINE)
  local index = y * qrsize + x
  return getBit(qrcode[(index rshift 3) + 1], (index & 7))
end

REM
REM Sets the module at the given coordinates, which must be in bounds.
REM
sub setModule(byref qrcode, x, y, isBlack)
  local qrsize = qrcode[0]
  assert(21 <= qrsize && qrsize <= 177 && 0 <= x && x < qrsize && 0 <= y && y < qrsize, PROGLINE)
  local index = (y * qrsize) + x
  local bitIndex = (index & 7)
  local byteIndex = ((index rshift 3) + 1)
  if (isBlack) then
    qrcode[byteIndex] |= (1 lshift bitIndex)
  else
    qrcode[byteIndex] &= ((1 lshift bitIndex) xor 0xFF)
  endif
end

REM
REM Sets the module at the given coordinates, doing nothing if out of bounds.
REM
sub setModuleBounded(byref qrcode, x, y, isBlack)
  local qrsize = qrcode[0]
  if (0 <= x && x < qrsize && 0 <= y && y < qrsize) then
    setModule(qrcode, x, y, isBlack)
  else
  endif
end

REM
REM Returns true iff the i-th bit of x is set to 1. Requires x >= 0 and 0 <= i <= 14.
REM
func getBit(x, i)
  return ((x rshift i) & 1) != 0
end

'---- Segment handling ----

REM
REM Tests whether the given string can be encoded as a segment in alphanumeric mode.
REM A string is encodable iff each character is in the following set: 0 to 9, A to Z
REM (uppercase only), space, dollar, percent, asterisk, plus, hyphen, period, slash, colon.
REM
func qrcodegen_isAlphanumeric(text)
  assert(text != 0, PROGLINE)
  local i, c
  for i = 0 to len(text) - 1
    c = mid(text, i + 1, 1)
    if (instr(ALPHANUMERIC_CHARSET, c) == 0) then
      return false
    endif
  next
  return true
end

REM
REM Tests whether the given string can be encoded as a segment in numeric mode.
REM A string is encodable iff each character is in the range 0 to 9.
REM
func qrcodegen_isNumeric(text)
  assert(text != 0, PROGLINE)
  local i, c
  for i = 0 to len(text) - 1
    c = mid(text, i + 1, 1)
    if (c < "0" || c > "9") then return false
  next
  return true
end

REM
REM Returns the number of bytes (uint8_t) needed for the data buffer of a segment
REM containing the given number of characters using the given mode. Notes:
REM - Returns SIZE_MAX on failure, i.e. numChars > INT16_MAX or
REM   the number of needed bits exceeds INT16_MAX (i.e. 32767).
REM - Otherwise, all valid results are in the range [0, ceil(INT16_MAX / 8)], i.e. at most 4096.
REM - It is okay for the user to allocate more bytes for the buffer than needed.
REM - For byte mode, numChars measures the number of bytes, not Unicode code points.
REM - For ECI mode, numChars must be 0, and the worst-case number of bytes is returned.
REM   An actual ECI segment can have shorter data. For non-ECI modes, the result is exact.
REM
func qrcodegen_calcSegmentBufferSize(mode, numChars)
  local temp = calcSegmentBitLength(mode, numChars)
  if (temp == -1) then return maxint
  assert(0 <= temp && temp <= maxint, PROGLINE)
  return (temp + 7) / 8
end

REM
REM Returns the number of data bits needed to represent a segment
REM containing the given number of characters using the given mode. Notes:
REM - Returns -1 on failure, i.e. numChars > INT16_MAX or
REM   the number of needed bits exceeds INT16_MAX (i.e. 32767).
REM - Otherwise, all valid results are in the range [0, INT16_MAX].
REM - For byte mode, numChars measures the number of bytes, not Unicode code points.
REM - For ECI mode, numChars must be 0, and the worst-case number of bits is returned.
REM   An actual ECI segment can have shorter data. For non-ECI modes, the result is exact.
REM
func calcSegmentBitLength(mode, numChars)
  ' All calculations are designed to avoid overflow on all platforms
  if (numChars > maxint) then return -1
  local result = numChars
  if (mode == qrcodegen_Mode_NUMERIC) then
    result = floor((result * 10 + 2) / 3)  ' ceil(10/3 * n)
  else if (mode == qrcodegen_Mode_ALPHANUMERIC)
    result = floor((result * 11 + 1) / 2)  ' ceil(11/2 * n)
  else if (mode == qrcodegen_Mode_BYTE)
    result *= 8
  else if (mode == qrcodegen_Mode_KANJI)
    result *= 13
  else if (mode == qrcodegen_Mode_ECI && numChars == 0)
    result = 3 * 8
  else
    ' Invalid argument
    assert(false, PROGLINE)
    return -1
  endif
  assert(result >= 0, PROGLINE)
  if (result > maxint) then return -1
  return result
end

REM
REM Returns a segment representing the given binary data encoded in
REM byte mode. All input byte arrays are acceptable. Any text string
REM can be converted to UTF-8 bytes and encoded as a byte mode segment.
REM
func qrcodegen_makeBytes(byref _data, dataLen, byref buf)
  assert(_data != 0 || dataLen == 0, PROGLINE)
  local result = {}
  result.mode = qrcodegen_Mode_BYTE
  result.bitLength = calcSegmentBitLength(result.mode, dataLen)
  assert(result.bitLength != -1, PROGLINE)
  result.numChars = dataLen
  if (dataLen > 0) then
    print "FIXME: memcpy"
    'memcpy(buf, data, len * sizeof(buf[0]))
  endif
  result._data = buf
  return result
end

REM
REM Returns a segment representing the given string of decimal digits encoded in numeric mode.
REM
func qrcodegen_makeNumeric(digits, byref buf)
  assert(digits != 0, PROGLINE)
  local result = {}
  local dataLen = len(digits)

  result.mode = qrcodegen_Mode_NUMERIC
  local bitLen = calcSegmentBitLength(result.mode, dataLen)
  assert(bitLen != -1, PROGLINE)
  result.numChars = dataLen
  if (bitLen > 0) then
    memset(buf, 0, (bitLen + 7) / 8)
  endif
  result.bitLength = 0

  local accumData = 0
  local accumCount = 0
  local digitIndex = 0

  while digitIndex < len(digits)
    local c = mid(digits, digitIndex + 1, 1)
    assert("0" <= c && c <= "9", PROGLINE)
    accumData = accumData * 10 + (c - "0")
    accumCount++
    if (accumCount == 3) then
      result.bitLength = appendBitsToBuffer(accumData, 10, buf, result.bitLength)
      accumData = 0
      accumCount = 0
    endif
    digitIndex++
  wend

  if (accumCount > 0) then
    ' 1 or 2 digits remaining
    result.bitLength = appendBitsToBuffer(accumData, accumCount * 3 + 1, buf, result.bitLength)
  endif
  assert(result.bitLength == bitLen, PROGLINE)
  result._data = buf
  return result
end

REM
REM Returns a segment representing the given text string encoded in alphanumeric mode.
REM The characters allowed are: 0 to 9, A to Z (uppercase only), space,
REM dollar, percent, asterisk, plus, hyphen, period, slash, colon.
REM
func qrcodegen_makeAlphanumeric(text, byref buf)
  assert(text != 0, PROGLINE)
  local result = {}
  local dataLen = len(text)

  result.mode = qrcodegen_Mode_ALPHANUMERIC
  local bitLen = calcSegmentBitLength(result.mode, dataLen)

  assert(bitLen != -1, PROGLINE)
  result.numChars = dataLen

  if (bitLen > 0) then
    memset(buf, 0, (bitLen + 7) / 8)
  endif

  result.bitLength = 0
  local accumData = 0
  local accumCount = 0
  local textIndex = 0

  while textIndex < len(text)
    local temp = instr(ALPHANUMERIC_CHARSET, mid(text, textIndex + 1, 1))
		accumData = accumData * 45 + temp - 1
    accumCount++
    if (accumCount == 2) then
      result.bitLength = appendBitsToBuffer(accumData, 11, buf, result.bitLength)
      accumData = 0
      accumCount = 0
    endif
    textIndex++
  wend

  if (accumCount > 0) then
    ' 1 character remaining
    result.bitLength = appendBitsToBuffer(accumData, 6, buf, result.bitLength)
  endif

  assert(result.bitLength == round(bitLen), PROGLINE)
  result._data = buf
  return result
end

REM
REM Returns a segment representing an Extended Channel Interpretation
REM (ECI) designator with the given assignment value.
REM
func qrcodegen_makeEci(assignVal, byref buf)
  local result = {}

  result.mode = qrcodegen_Mode_ECI
  result.numChars = 0
  result.bitLength = 0
  if (assignVal < 0) then
    assert(false, PROGLINE)
  else if (assignVal < (1 lshift 7)) then
    memset(buf, 0, 1)
    result.bitLength = appendBitsToBuffer(assignVal, 8, buf, result.bitLength)
  else if (assignVal < (1 lshift 14))
    memset(buf, 0, 2)
    result.bitLength = appendBitsToBuffer(2, 2, buf, result.bitLength)
    result.bitLength = appendBitsToBuffer(assignVal, 14, buf, result.bitLength)
  else if (assignVal < 1000000)
    memset(buf, 0, 3)
    result.bitLength = appendBitsToBuffer(6, 3, buf, result.bitLength)
    result.bitLength = appendBitsToBuffer((assignVal rshift 10), 11, buf, result.bitLength)
    result.bitLength = appendBitsToBuffer((assignVal & 0x3FF), 10, buf, result.bitLength)
  else
    assert(false, PROGLINE)
  endif
  result._data = buf
  return result
end

REM
REM Calculates the number of bits needed to encode the given segments at the given version.
REM Returns a non-negative number if successful. Otherwise returns -1 if a segment has too
REM many characters to fit its length field, or the total bits exceeds INT16_MAX.
REM
func getTotalBits(byref segs, segsLen, version)
  assert(isarray(segs) && lenSegs > 0, PROGLINE)
  local result = 0
  local i

  for i = 0 to segsLen - 1
    local numChars  = segs[i].numChars
    local bitLength = segs[i].bitLength
    assert(0 <= numChars && numChars  <= INT16_MAX, PROGLINE)
    assert(0 <= bitLength && bitLength <= INT16_MAX, PROGLINE)
    local ccbits = numCharCountBits(segs[i].mode, version)
    assert(0 <= ccbits && ccbits <= 16, PROGLINE)
    if (numChars >= (1 lshift ccbits)) then return -1  ' The segment's length doesn't fit the field's bit width
    result += 4 + ccbits + bitLength
    if (result > maxint) then return -1  ' The sum might overflow an int type
  next i
  assert(0 <= result && result <= INT16_MAX, PROGLINE)
  return result
end

REM
REM Returns the bit width of the character count field for a segment in the given mode
REM in a QR Code at the given version number. The result is in the range [0, 16].
REM
func numCharCountBits(mode, version)
  assert(qrcodegen_VERSION_MIN <= version && version <= qrcodegen_VERSION_MAX, PROGLINE)
  local i = (version + 7) / 17
  local m
  select case mode
  case qrcodegen_Mode_NUMERIC     : m = [10, 12, 14]: return m[i]
  case qrcodegen_Mode_ALPHANUMERIC: m = [9, 11, 13]:  return m[i]
  case qrcodegen_Mode_BYTE        : m = [8, 16, 16]:  return m[i]
  case qrcodegen_Mode_KANJI       : m = [8, 10, 12]:  return m[i]
  case qrcodegen_Mode_ECI         : return 0
  case else: print "Invalid mode: " + mode: assert(false, PROGLINE)
  end select
end

'sub test_reedSolomonMultiply
'  for x = 0 to 9
'    for y = 0 to 9
'      print reedSolomonMultiply(x, y) +  " ";
'    next y
'    print "\n"
'  next x
'  pause
'end
'
'sub test_numCharCountBits
'  print numCharCountBits(qrcodegen_Mode_BYTE, qrcodegen_VERSION_MIN)
'end
'
'sub test_memmove
'  local v = [1,2,3,4,5,6,7,8]
'  local expected = [5,6,7,8,5,6,7,8]
'  memmove(v, 0, 4, 4)
'  local i
'  for i = 0 to 7
'    assert(v[i] == expected[i], PROGLINE)
'  next i
'  memset(v, 777, 8)
'  for i = 0 to 7
'    assert(v[i] == 777, PROGLINE)
'  next i
'end
'
'test_memmove
'test_reedSolomonMultiply
'test_numCharCountBits
'assert(qrcodegen_isNumeric("1234"), PROGLINE)
'assert(qrcodegen_isAlphanumeric("abc123") == false, PROGLINE)
'assert(qrcodegen_isAlphanumeric("ABC123"), PROGLINE)
'pause

'sub main(text)
'  local bufLen = qrcodegen_BUFFER_LEN_FOR_VERSION(qrcodegen_VERSION_MAX)
'  dim qrcode(bufLen)
'  dim tempBuffer(bufLen)
'  local errCorLvl = qrcodegen_Ecc_LOW
'  if (!qrcodegen_encodeText(text, tempBuffer, qrcode, errCorLvl, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true)) then
'    print "Failed to create QR code"
'  else
'    local size = qrcodegen_getSize(qrcode)
'    local border = 4
'    local x, y
'    for y = -border to size + border - 1
'      for x = -border to  size + border - 1
'        print iff(qrcodegen_getModule(qrcode, x, y), "##", "  ");
'      next x
'      print
'    next y
'    print
'  endif
'end
'
'main("123")
'main("123ABC")
'main("cats")