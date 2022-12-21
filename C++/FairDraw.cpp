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
 * @version 1.3
 * @see https://github.com/CuiZhenhang/Fair-Draw
 * 编译命令：`g++ .\FairDraw.cpp .\md5.cpp -std=c++14 -fexec-charset=GBK -o FairDraw`
 */

#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <random>
#include <chrono>
#include <stdexcept>
#include <cstring>
#include <cmath>
#include "md5.h"

namespace FairDraw {
	namespace Utils {
		template<class T> inline T& trim (T &str);
		std::wstring gbkToUnicode (const std::string &str);
	} // namespace Utils

	/**
	 * 生成 64 位无符号伪随机数。
	 * @param seed 64 位无符号随机数种子，可传入上次的伪随机数。
	 * @returns 新的 64 位无符号伪随机数。
	 */
	uint64_t nextRandomInt64 (uint64_t seed);

	/**
	 * 计算 64 位下 x 的 k 次方。时间复杂度 $O(\log(k))$。
	 */
	uint64_t bit64Power (uint64_t x, uint64_t k);
	
	/**
	 * 获取前 `number` 个质数从小到大排列的数组。时间复杂度约 $O(number \log(number))$。
	 */
	std::vector<uint64_t> getPrimes (uint64_t number);

	/**
	 * 通过一个数组计算 64 位种子。
	 * @param array 非空数组，每一项都是一个非负数。
	 * @returns 计算出的 64 位种子。
	 */
	uint64_t getSeedFromArray (const std::vector<uint64_t> &array);

	/**
	 * 获取一串文字的编码数组。
	 * @param text 输入的文字，非空，忽略首尾空白字符。
	 * @returns 计算出的编码数组。
	 */
	std::vector<uint64_t> getArrayFromString (const std::string &text);

	/**
	 * 生成一个随机字符串，由“a-z”、“A-Z”、“0-9”共 62 种字符组成。
	 * @param length 正整数，字符串的长度，默认为 64。
	 * @returns 生成的随机字符串。
	 */
	std::string getRandomText (uint64_t length = 64);

	/**
	 * 通过给定参数获取随机排名。相同的输入有相同的输出。
	 * @param text 参数字符串，非空。
	 * @param params 参数数组，可以为空数组。
	 * @param total 最大排名。
	 * @param nth 使用生成的第哪些个随机数，默认仅含一个数 100。
	 * @returns 得到的随机排名，范围从 1 到 `total`。
	 */
	std::vector<uint64_t> getRandomRanks (const std::string &text, const std::vector<uint64_t> &params, uint64_t total, const std::vector<uint64_t> &nth = { 100 });
} // namespace FairDraw

template<class T>
inline int readline (T &x, const char query[] = nullptr) {
	if (query) std::cout << query;
	std::string line;
	getline(std::cin, line);
	FairDraw::Utils::trim(line);
	if (line.empty()) return 1;
	std::stringstream(line) >> x;
	return 0;
}

template<class T>
inline int readline (std::vector<T> &array, const char query[] = nullptr) {
	if (query) std::cout << query;
	std::string line;
	getline(std::cin, line);
	FairDraw::Utils::trim(line);
	if (line.empty()) return 1;
	std::stringstream stream(line);
	T temp;
	while (stream >> temp) array.emplace_back(temp);
	return 0;
}

int main () {
	using std::cout;
	using std::endl;
	cout << "欢迎使用 Fair Draw，当前版本 1.3，开源链接：https://github.com/CuiZhenhang/Fair-Draw。" << endl;
	cout << "操作模式有：\n1. 生成字符串；\n2. 生成随机排名；\n3. 检验哈希值。" << endl;
	uint32_t code;
	readline(code, "请输入操作模式前的数字：");
	switch (code) {
		case 1: {
			int64_t length;
			if (readline(length, "请输入字符串长度（默认为64）：")) length = 64;
			if (length <= 0) throw std::out_of_range("长度不合法");
			std::string text = FairDraw::getRandomText(length);
			cout << "生成的参数字符串为：" << text << endl;
			cout << "字符串 md5 哈希值为：" << MD5(text).toStr() << endl;
			break;
		}
		case 2: {
			std::string text;
			std::vector<uint64_t> params;
			int64_t total;
			std::vector<int64_t> signed_nth;
			std::vector<uint64_t> nth;
			if (readline(text, "请输入参数字符串：")) throw std::length_error("参数字符串应当非空");
			readline(params, "请输入参数列表：");
			readline(total, "请输入最大排名：");
			if (total <= 0) throw std::out_of_range("最大排名不合法");
			if (readline(signed_nth, "使用第哪些个随机数（默认仅 100）：")) signed_nth = { 100 };
			nth.reserve(signed_nth.size());
			for (int64_t value : signed_nth) nth.emplace_back(value < 0 ? 0 : value);
			std::vector<uint64_t> ranks = FairDraw::getRandomRanks(text, params, total, nth);
			cout << "生成的随机排名为：";
			for (uint64_t rank : ranks) cout << rank << ' ';
			cout << endl;
			break;
		}
		case 3: {
			std::string text, hash;
			if (readline(text, "请输入参数字符串：")) throw std::length_error("参数字符串应当非空");
			readline(hash, "请输入 md5 哈希值：");
			std::string result = MD5(text).toStr();
			if (result == hash) cout << "比对成功" << endl;
			else cout << "比对失败，参数字符串的 md5 为：" << result << endl;
			break;
		}
		default: throw std::invalid_argument("输入不合法");
	}
	return 0;
}

namespace FairDraw {
	namespace Utils {
		template<class T>
		inline T& trim (T &str) {
			static const auto notToTrim = [](uint32_t code) -> bool {
				if (code == 0) return false;
				return !isspace(code);
			};
			auto front = std::find_if(str.begin(), str.end(), notToTrim);
			str.erase(str.begin(), front);
			auto back = std::find_if(str.rbegin(), str.rend(), notToTrim);
			str.erase(back.base(), str.end());
			return str;
		}

		std::wstring gbkToUnicode (const std::string &str) {
			size_t len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
			std::unique_ptr<wchar_t[]> result(new wchar_t[len + 1]);
			memset(result.get(), 0, sizeof (wchar_t) * (len + 1));
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, result.get(), len);
			return result.get();
			/*
			// ansi to unicode, require `std::setlocale("");`
			std::wstring result;
			std::mbstate_t state;
			const char *src = str.data();
			size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
#			ifdef DEBUG
			std::cerr << "init len=" << len << std::endl;
#			endif
			if (static_cast<size_t>(-1) != len) {
				std::unique_ptr<wchar_t[]> buf(new wchar_t[len + 1]);
				memset(buf.get(), 0, sizeof (wchar_t) * (len + 1));
				len = std::mbsrtowcs(buf.get(), &src, len, &state);
				if (static_cast<size_t>(-1) != len) {
					result.assign(buf.get(), wcslen(buf.get()));
				}
#				ifdef DEBUG
				std::cerr << "buf: ";
				for (size_t i = 0; i < len; ++i) std::cerr << (uint32_t) *(buf.get() + i) << ' ';
				std::cerr << std::endl;
#				endif
			}
#			ifdef DEBUG
			std::cerr << "result: ";
			for (wchar_t code : result) std::cerr << (uint32_t) code << ' ';
			std::cerr << std::endl;
#			endif
			return result;
			*/
		}
	}

	uint64_t nextRandomInt64 (uint64_t seed) {
		seed ^= seed << 32;
		seed ^= seed >> 13;
		seed ^= seed << 1;
		return seed;
	}

	uint64_t bit64Power (uint64_t x, uint64_t k) {
		uint64_t result = 1;
		while (k) {
			if (k & 1) result *= x;
			x *= x;
			k >>= 1;
		}
		return result;
	}

	std::vector<uint64_t> getPrimes (uint64_t number) {
		if (number == 0) return {};
		uint64_t max = round(number * log((double) number) * 1.5);
		std::vector<uint64_t> primes;
		std::vector<uint8_t> notPrime(max + 1);
		primes.reserve(number);
		notPrime[0] = notPrime[1] = true;
		for (uint64_t x = 2; x <= max; ++x) {
			if (!notPrime[x]) {
				primes.emplace_back(x);
				if (primes.size() == number) {
					return primes;
				}
			}
			uint64_t _mx = max / x;
			for (uint64_t prime : primes) {
				if (prime > _mx) break;
				notPrime[x * prime] = true;
				if (x % prime == 0) break;
			}
		}
		while (primes.size() < number) {
			if (++max == 0) break;
			bool isPrime = true;
			uint64_t _mx = sqrtl((long double) max);
			for (uint64_t prime : primes) {
				if (prime > _mx) break;
				if (max % prime == 0) {
					isPrime = false;
					break;
				}
			}
			if (isPrime) primes.emplace_back(max);
		}
		return primes;
	}

	uint64_t getSeedFromArray (const std::vector<uint64_t> &array) {
		if (array.empty()) throw std::length_error("getSeedFromArray(): array 应当非空");
		std::vector<uint64_t> primes = getPrimes(array.size() + 1);
		size_t index = 1;
		uint64_t seed = 1;
		for (uint64_t value : array) {
			seed *= bit64Power(primes[index++], value);
		}
		return seed;
	}

	std::vector<uint64_t> getArrayFromString (const std::string &text) {
		std::string textTrim = text;
		Utils::trim(textTrim);
		if (textTrim.empty()) throw std::length_error("getArrayFromString(): text 应当非空");
		std::wstring wtext = Utils::gbkToUnicode(textTrim);
		std::vector<uint64_t> array;
		for (wchar_t code : wtext) {
			array.emplace_back(code);
		}
		return array;
	}

	std::string getRandomText (uint64_t length) {
		if (length == 0) throw std::out_of_range("getRandomText(): length 应当大于 0");
		static const std::string MODE = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		static std::mt19937 random(std::chrono::system_clock::now().time_since_epoch().count());
		std::string result;
		for (uint64_t i = 0; i < length; ++i) {
			uint32_t index = random() % 62;
			result += MODE[index];
		}
		return result;
	}

	std::vector<uint64_t> getRandomRanks (const std::string &text, const std::vector<uint64_t> &params, uint64_t total, const std::vector<uint64_t> &nth) {
		std::string textTrim = text;
		Utils::trim(textTrim);
		if (textTrim.empty()) throw std::length_error("getRandomRanks(): text 应当非空");
		if (total == 0) throw std::out_of_range("getRandomRanks(): total 应当大于 0");
		std::vector<uint64_t> array = getArrayFromString(textTrim);
		array.insert(array.begin(), params.cbegin(), params.cend());
		uint64_t seed = getSeedFromArray(array);
		std::map<uint64_t, uint64_t> need;
		uint64_t maxNth = 0;
		for (uint64_t value : nth) {
			need[value] = true;
			if (value > maxNth) maxNth = value;
		}
		if (need[0]) need[0] = seed % total + 1;
		for (uint64_t times = 0; times < maxNth; ++times) {
			seed = nextRandomInt64(seed);
			if (need[times + 1]) need[times + 1] = seed % total + 1;
		}
		std::vector<uint64_t> result;
		result.reserve(nth.size());
		for (uint64_t value : nth) {
			result.emplace_back(need[value]);
		}
		return result;
	}
} // namespace FairDraw
