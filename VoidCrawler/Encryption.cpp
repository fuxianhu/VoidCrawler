#include "Encryption.h"
#include "Core.h"

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
#include <cryptopp/osrng.h>
#include <QByteArray>


void EncryptionModel::DeriveKeyAndIV(const std::string & password, CryptoPP::SecByteBlock & key, CryptoPP::SecByteBlock & iv)
 {
        // 使用 SHA-256 哈希函数从密码生成一个 32 字节（256 位）的密钥
        CryptoPP::SHA256 hash;
    hash.CalculateDigest(key, (const byte*)password.data(), password.size());
   
            // 简单地从密钥派生一个 16 字节（128 位）的IV：取密钥的前 16 字节
        iv.resize(CryptoPP::AES::BLOCKSIZE);
    memcpy(iv, key, CryptoPP::AES::BLOCKSIZE);
    }
std::string EncryptionModel::EncryptString(const std::string & plaintext, const std::string & password)
{
    CryptoPP::SecByteBlock key(CryptoPP::AES::MAX_KEYLENGTH); // AES-256 密钥长度
    CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);      // AES 块大小，即 IV 长度
    DeriveKeyAndIV(password, key, iv);
   
        std::string ciphertext;
    try
    {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryptor;
        encryptor.SetKeyWithIV(key, key.size(), iv);
       
                  // 构建处理管道：明文 -> 加密 -> Base64 编码 -> 字符串
            CryptoPP::StringSource ss(plaintext, true,
                new CryptoPP::StreamTransformationFilter(encryptor,
                    new CryptoPP::Base64Encoder(
                        new CryptoPP::StringSink(ciphertext),
                        false // 不在 Base64 输出中添加换行
                        )
                    )
                );
        }
    catch (const CryptoPP::Exception& e)
        {
         std::cerr << "加密错误: " << e.what() << std::endl;
         return "";
       }
    return ciphertext;
    }

 std::string EncryptionModel::DecryptString(const std::string & ciphertextBase64, const std::string & password)
 {
  CryptoPP::SecByteBlock key(CryptoPP::AES::MAX_KEYLENGTH);
    CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
 DeriveKeyAndIV(password, key, iv);
  
      std::string decryptedtext;
   try
    {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryptor;
         decryptor.SetKeyWithIV(key, key.size(), iv);
       
               // 构建处理管道：Base64 字符串 -> 解码 -> 解密 -> 明文
         CryptoPP::StringSource ss(ciphertextBase64, true,
               new CryptoPP::Base64Decoder(
                   new CryptoPP::StreamTransformationFilter(decryptor,
                     new CryptoPP::StringSink(decryptedtext)
                       )
                    )
               );
       }
   catch (const CryptoPP::Exception& e)
       {
        std::cerr << "解密错误: " << e.what() << std::endl;
       return "";
        }
   return decryptedtext;
    }

 QString EncryptionModel::SecBlockToHexQString(const CryptoPP::SecByteBlock & block)
{
   std::string hex;
   CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hex), false /* uppercase=false */);
  encoder.Put(block.data(), block.size());
   encoder.MessageEnd();
   return QString::fromStdString(hex);
  }

 CryptoPP::SecByteBlock EncryptionModel::HexQStringToSecBlock(const QString & hexStr)
{
    std::string hex = hexStr.toStdString();
   CryptoPP::SecByteBlock block(hex.size() / 2);
    CryptoPP::StringSource(
    hex, true,
       new CryptoPP::HexDecoder(new CryptoPP::ArraySink(block, block.size()))
      );
    return block;
   }

QByteArray EncryptionModel::StdStringToQByteArray(const std::string & str)
{
    return QByteArray::fromRawData(str.data(), static_cast<int>(str.size()));
  }

 QPair<QString, QString> EncryptionModel::GenerateSaltedSHA256(const QString& password)
{
      // 1. 生成 32 字节随机盐
        CryptoPP::AutoSeededRandomPool rng;
 CryptoPP::SecByteBlock salt(32);
    rng.GenerateBlock(salt, salt.size());
   
        // 2. 准备输入：salt + password (UTF-8)
     QByteArray passwordUtf8 = password.toUtf8();
    std::string input;
     input.reserve(salt.size() + passwordUtf8.size());
 
       input.append(reinterpret_cast<const char*>(salt.data()), salt.size());
     input.append(passwordUtf8.constData(), passwordUtf8.size());

     // 3. 计算 SHA-256
        CryptoPP::SHA256 hash;
   byte digest[CryptoPP::SHA256::DIGESTSIZE];
    hash.CalculateDigest(digest, reinterpret_cast<const byte*>(input.data()), input.size());

  // 4. 转为 Hex QString
CryptoPP::SecByteBlock hashBlock(digest, CryptoPP::SHA256::DIGESTSIZE);
QString hashHex = SecBlockToHexQString(hashBlock);
 QString saltHex = SecBlockToHexQString(salt);
  
       return qMakePair(hashHex, saltHex);
    }

 bool EncryptionModel::VerifySaltedSHA256(const QString& storedHashHex, const QString& inputPassword, const QString& saltHex)
 {
     try
     {
         // 1. 解码盐
         CryptoPP::SecByteBlock salt = HexQStringToSecBlock(saltHex);

         // 2. 重建输入：salt + inputPassword (UTF-8)
         QByteArray passwordUtf8 = inputPassword.toUtf8();
         std::string input;
         input.reserve(salt.size() + passwordUtf8.size());

         input.append(reinterpret_cast<const char*>(salt.data()), salt.size());
         input.append(passwordUtf8.constData(), passwordUtf8.size());

         // 3. 计算 SHA-256
         CryptoPP::SHA256 hash;
         byte digest[CryptoPP::SHA256::DIGESTSIZE];
         hash.CalculateDigest(digest, reinterpret_cast<const byte*>(input.data()), input.size());

         // 4. 转为 Hex 并比较
         CryptoPP::SecByteBlock computedHash(digest, CryptoPP::SHA256::DIGESTSIZE);
         QString computedHex = SecBlockToHexQString(computedHash);

         return computedHex.compare(storedHashHex, Qt::CaseInsensitive) == 0;
     }
     catch (const CryptoPP::Exception& e)
     {
         VCCore::logger->warn(
             std::format("In EncryptionModel::VerifySaltedSHA256({}, {}, {}) function, Exception: {} . So function will return false. ",
                 storedHashHex.toStdString(), inputPassword.toStdString(), saltHex.toStdString(), std::string(e.what())));
         // 解码失败或其它异常，视为验证失败
         return false;
     }
 }
 #ifdef QT_DEBUG
void EncryptionModel::test(std::string password,
    std::string originalText)
 {
   OutputDebugStringW(L"原文: ");
    OutputDebugStringW(std::wstring(originalText.begin(), originalText.end()).c_str());
    OutputDebugStringW(L"\n");
   
        // 加密
       std::string encrypted = EncryptionModel::EncryptString(originalText, password);
   OutputDebugStringW(L"加密后 (Base64): ");
   OutputDebugStringW(std::wstring(encrypted.begin(), encrypted.end()).c_str());
  OutputDebugStringW(L"\n");
  
     // 解密
        std::string decrypted = EncryptionModel::DecryptString(encrypted, password);
    OutputDebugStringW(L"解密后: ");
     OutputDebugStringW(std::wstring(decrypted.begin(), decrypted.end()).c_str());
  OutputDebugStringW(L"\n");
 
          // 验证
         if (originalText == decrypted)
         {
        OutputDebugStringW(L"加解密成功！\n");
       }
   else
      {
       OutputDebugStringW(L"加解密失败！\n");
     }
 }
#endif