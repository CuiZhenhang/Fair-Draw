/*
Copyright 2022 CuiZhenhang (github.com/CuiZhenhang)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/**
 * @name 公平抽奖助手
 * @author CuiZhenhang
 * @version 1.2
 * @see https://github.com/CuiZhenhang/Fair-Draw
 */

// @ts-check
const { stdin, stdout } = require('process')
const readline = require('readline')
const crypto = require('crypto')

/**
 * 生成 64 位无符号伪随机数。
 * @param { bigint } seed 64 位无符号随机数种子，可传入上次的伪随机数。
 * @returns { bigint } 新的 64 位无符号伪随机数。
 */
function nextRandomInt64 (seed) {
    if (typeof seed !== 'bigint') throw new TypeError("seed 应当传入的类型为 bigint")
    const BIT64 = (1n << 64n) - 1n
    seed &= BIT64
    seed ^= (seed << 32n) & BIT64
    seed ^= (seed >> 13n) & BIT64
    seed ^= (seed << 1n) & BIT64
    return seed
}

/**
 * 计算 64 位下 x 的 k 次方。时间复杂度 $O(\log(k))$。
 * @param { bigint } x 
 * @param { bigint } k 无符号整数。
 * @returns { bigint } 64 位下 x 的 k 次方。
 */
function bit64Power (x, k) {
    if (typeof x !== 'bigint') throw new TypeError("x 应当传入的类型为 bigint")
    if (typeof k !== 'bigint') throw new TypeError("k 应当传入的类型为 bigint")
    if (k < 0) throw new RangeError("k 应当传入无符号整数")
    const BIT64 = (1n << 64n) - 1n
    x &= BIT64
    let result = 1n
    while (k) {
        if (k & 1n) result = (result * x) & BIT64
        x = (x * x) & BIT64
        k >>= 1n
    }
    return result
}

/**
 * 获取前 `number` 个质数从小到大排列的数组。时间复杂度约 $O(number \log(number))$。
 * @param { number } number 向下取整。若为负数，则视为 0。
 * @returns { Array<number> }
 */
function getPrimes (number) {
    if (typeof number !== 'number') throw new TypeError("number 应当传入的类型为 number")
    if (!Number.isFinite(number)) throw new RangeError("number 应当为有限数字")
    number = Math.floor(number)
    if (number <= 0) return []
    let max = Math.round(number * Math.log(number) * 1.5)
    let primes = []
    let notPrime = [true, true]
    for (let x = 2; x <= max; ++x) {
        if (!notPrime[x]) {
            if (primes.push(x) === number) {
                return primes
            }
        }
        for (let prime of primes) {
            if (x * prime > max) break
            notPrime[x * prime] = true
            if (x % prime === 0) break
        }
    }
    while (primes.length < number) {
        ++max
        let isPrime = true
        for (let prime of primes) {
            if (prime * prime > max) break
            if (max % prime === 0) {
                isPrime = false
                break
            }
        }
        if (isPrime) primes.push(max)
    }
    return primes
}

/**
 * 通过一个数组计算 64 位种子。
 * @param { Array<number|bigint> } array 非空数组，每一项都是一个非负数。
 * @returns { bigint } 计算出的 64 位种子。
 */
function getSeedFromArray (array) {
    if (!Array.isArray(array)) throw new TypeError("array 应当传入一个数组")
    if (array.length < 1) throw new RangeError("array 应当传入非空数组")
    if (array.some((value) => typeof value !== 'bigint' && typeof value !== 'number')) {
        throw new TypeError("array 的每个元素的类型都应当为 number 或 bigint")
    }
    if (array.some((value) => value < 0 || (typeof value === 'number' && !Number.isFinite(value)))) {
        throw new RangeError("array 的每个元素都应当为非负有限数字")
    }
    const BIT64 = (1n << 64n) - 1n
    let bigintArray = array.map((value) => typeof value === 'bigint' ? value : BigInt(Math.floor(value)))
    // 大于等于 3 的质数数组。
    let primes = getPrimes(bigintArray.length + 1).map((value) => BigInt(value))
    primes.shift()
    return bigintArray.reduce((result, value, index) => {
        return (result * bit64Power(primes[index], value)) & BIT64
    }, 1n)
}

/**
 * 获取一串文字的编码数组。
 * @param { string } text 输入的文字，非空，忽略首尾空白字符。
 * @returns { Array<number> } 计算出的编码数组。
 */
function getArrayFromString (text) {
    if (typeof text !== 'string') throw new TypeError("text 应当传入的类型为 string")
    let array = []
    for (let char of text.trim()) {
        let code = char.codePointAt(0)
        if (typeof code === 'number') {
            array.push(code)
        }
    }
    if (array.length < 1) throw new RangeError("text 应当传入非空字符串")
    return array
}

/**
 * 生成一个随机字符串，由“a-z”、“A-Z”、“0-9”共 62 种字符组成。
 * @param { number } length 正整数，字符串的长度，默认为 64。
 * @returns { string } 生成的随机字符串。
 */
function getRandomText (length = 64) {
    if (typeof length !== 'number') throw new TypeError("length 应当传入的类型为 number")
    if (!Number.isFinite(length)) throw new RangeError("length 应当为有限数字")
    length = Math.floor(length)
    if (length < 1) throw new RangeError("length 应当传入正整数")
    const MODE = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
    let result = ""
    for (let i = 1; i <= length; ++i) {
        let index = Math.floor(Math.random() * MODE.length)
        result += MODE[index]
    }
    return result
}

/**
 * 通过给定参数获取随机排名。相同的输入有相同的输出。
 * @param { string } text 参数字符串，非空。
 * @param { Array<number|bigint> } params 参数数组，可以为空数组。
 * @param { number } total 最大排名。
 * @param { Array<number> } nth 使用生成的第哪些个随机数，默认仅含一个数 100。负数视为 0。
 * @returns { Array<number> } 得到的随机排名，范围从 1 到 `total`。
 */
function getRandomRanks (text, params, total, nth = [100]) {
    if (typeof text !== 'string') throw new TypeError("text 应当传入的类型为 string")
    if (text.trim().length < 1) throw new RangeError("text 应当传入非空字符串")
    if (!Array.isArray(params)) throw new TypeError("array 应当传入一个数组")
    if (typeof total !== 'number') throw new TypeError("total 应当传入的类型为 number")
    if (total < 1) throw new RangeError("total 应当是一个正整数")
    if (!Array.isArray(nth)) throw new TypeError("nth 应当传入一个数组")
    if (nth.some((value) => typeof value !== 'number')) throw new TypeError("nth 的每个元素的类型都应当为 number")
    if (nth.some((value) => !Number.isFinite(value))) throw new RangeError("nth 的每个元素都应当为有限数字")
    let array = params.concat(getArrayFromString(text))
    let seed = getSeedFromArray(array)
    let need = [], maxNth = 0
    for (let value of nth) {
        value = value < 0 ? 0 : Math.floor(value)
        need[value] = true
        if (value > maxNth) maxNth = value
    }
    if (need[0]) need[0] = Number(seed % BigInt(total) + 1n)
    for (let times = 1; times <= maxNth; ++times) {
        seed = nextRandomInt64(seed)
        if (need[times]) need[times] = Number(seed % BigInt(total) + 1n)
    }
    return nth.map((value) => need[value])
}

function main () {
    const io = readline.createInterface({
        input: stdin,
        output: stdout
    })
    io.write("操作模式有：\n1. 生成字符串；\n2. 生成随机排名；\n3. 检验哈希值。\n")
    io.question("请输入操作模式前的数字：", (answer) => {
        let code = Number(answer)
        switch (code) {
            case 1: {
                io.question("请输入字符串长度（默认为64）：", (answer) => {
                    answer = answer.trim()
                    let length = answer.length ? Math.floor(Number(answer)) : 64
                    if (!Number.isFinite(length) || length < 1) throw new Error("长度不合法")
                    let text = getRandomText(length)
                    io.write(`生成的参数字符串为：${ text }\n`)
                    io.write(`字符串 md5 哈希值为：${ crypto.createHash('md5').update(text).digest('hex') }\n`)
                    io.close()
                })
                break
            }
            case 2: {
                io.question("请输入参数字符串：", (text) => {
                    text = text.trim()
                    if (text.length < 1) throw new Error("参数字符串应当非空")
                    io.question("请输入参数列表：", (textParams) => {
                        let params = textParams.trim().split(/\s+/).map((value) => Number(value))
                        if (params.some((value) => value < 0 || !Number.isFinite(value))) throw new Error("参数列表不合法")
                        io.question("请输入最大排名：", (textTotal) => {
                            let total = Math.floor(Number(textTotal))
                            if (!Number.isFinite(total) || total < 1) throw new Error("最大排名不合法")
                            io.question("使用第哪些个随机数（默认仅 100）：", (textNth) => {
                                textNth = textNth.trim()
                                let nth = textNth.length ? textNth.split(/\s+/).map((value) => Number(value)) : [100]
                                if (nth.some((value) => !Number.isFinite(value))) throw new Error("数字不合法")
                                let ranks = getRandomRanks(text, params, total, nth)
                                io.write(`生成的随机排名为：${ ranks.join(' ') }\n`)
                                io.close()
                            })
                        })
                    })
                })
                break
            }
            case 3: {
                io.question("请输入参数字符串：", (text) => {
                    text = text.trim()
                    if (text.length < 1) throw new Error("参数字符串应当非空")
                    io.question("请输入 md5 哈希值：", (hash) => {
                        let result = crypto.createHash('md5').update(text).digest('hex')
                        if (result === hash.trim()) io.write("比对成功\n")
                        else io.write(`比对失败，参数字符串的 md5 为：${ result }\n`)
                        io.close()
                    })
                })
                break
            }
            default: throw new Error("输入不合法")
        }
    })
}

main()
