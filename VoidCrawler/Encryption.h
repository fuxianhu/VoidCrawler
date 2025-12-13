
#pragma once
#include <windows.h>
#include <string>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/secblock.h>
#include <QObject>
#include <QString>
#include <QPair>


class EncryptionModel
{
public:
	// 使用用户提供的字符串密钥生成固定长度的 AES 密钥和 IV
	static void DeriveKeyAndIV(const std::string& password, CryptoPP::SecByteBlock& key, CryptoPP::SecByteBlock& iv);

	// 加密字符串，返回 Base64 编码的结果
	static std::string EncryptString(const std::string& plaintext, const std::string& password);

	// 解密 Base64 编码的密文字符串
	static std::string DecryptString(const std::string& ciphertextBase64, const std::string& password);

	// 将 SecByteBlock 转为 Hex QString
	static QString SecBlockToHexQString(const CryptoPP::SecByteBlock& block);

	// 将 Hex QString 转为 SecByteBlock
	static CryptoPP::SecByteBlock HexQStringToSecBlock(const QString& hexStr);

	// 将 std::string 转为 QByteArray（避免本地编码问题）
	static QByteArray StdStringToQByteArray(const std::string& str);

	// 生成带盐 SHA-256，返回 [哈希结果, 盐]
	static QPair<QString, QString> GenerateSaltedSHA256(const QString& password);

	// 验证密码
	static bool VerifySaltedSHA256(const QString& storedHashHex, const QString& inputPassword, const QString& saltHex);
#ifdef QT_DEBUG
	// 测试函数，构建模式中不会编译
	static void test(std::string password = "MySecretPassword123!",
		std::string originalText = "Hello, this is a secret message!");
#endif
};