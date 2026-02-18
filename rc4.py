#!/usr/bin/env python3
"""
RC4 Payload Encryptor
Encrypts shellcode/payload files with RC4 using a password key
"""

import sys
import os
from Crypto.Cipher import ARC4
from Crypto.Hash import SHA256

def encrypt_payload(input_file, output_file, password):
    """
    Encrypt a payload file using RC4 with SHA256 hashed password
    
    Args:
        input_file: Path to plain shellcode/payload
        output_file: Path to save encrypted output
        password: Password string for encryption key
    """
    
    print(f"[*] Reading payload from: {input_file}")
    
    # Read the payload
    try:
        with open(input_file, 'rb') as f:
            plaintext = f.read()
    except FileNotFoundError:
        print(f"[-] Error: File '{input_file}' not found!")
        return False
    except Exception as e:
        print(f"[-] Error reading file: {e}")
        return False
    
    print(f"[+] Payload size: {len(plaintext)} bytes")
    
    # Hash password with SHA256 to get 32-byte key
    print(f"[*] Deriving RC4 key from password (SHA256)...")
    key_hash = SHA256.new(password.encode('utf-8'))
    key = key_hash.digest()
    
    print(f"[*] Key hash: {key.hex()}")
    
    # Create RC4 cipher
    cipher = ARC4.new(key)
    
    # Encrypt (RC4 is symmetric - no padding needed!)
    print(f"[*] Encrypting with RC4...")
    ciphertext = cipher.encrypt(plaintext)
    
    print(f"[+] Encrypted size: {len(ciphertext)} bytes")
    
    # Write encrypted file (just the ciphertext, no salt/IV needed)
    try:
        with open(output_file, 'wb') as f:
            f.write(ciphertext)
        print(f"[+] Encrypted payload saved to: {output_file}")
        print(f"[+] Total file size: {len(ciphertext)} bytes")
    except Exception as e:
        print(f"[-] Error writing file: {e}")
        return False
    
    # Show first bytes for verification
    print(f"\n[*] First 32 bytes of encrypted payload:")
    print(f"    {ciphertext[:32].hex()}")
    
    print(f"\n[✓] Encryption complete!")
    print(f"[!] Remember your password: '{password}'")
    print(f"[!] You'll need it to decrypt with earlybird.exe\n")
    
    return True


def main():
    if len(sys.argv) != 4:
        print("=" * 60)
        print("  RC4 Payload Encryptor (Simple & Fast)")
        print("=" * 60)
        print(f"\nUsage: {sys.argv[0]} <input.bin> <output.bin> <password>")
        print("\nExamples:")
        print(f"  python {sys.argv[0]} payload.bin encrypted.bin MySecretPass123")
        print(f"  python {sys.argv[0]} meterpreter.bin enc.bin P@ssw0rd!2024")
        print("\nParameters:")
        print("  <input.bin>   - Plain shellcode/payload file")
        print("  <output.bin>  - RC4 encrypted output file")
        print("  <password>    - Encryption password (use same in earlybird.exe)")
        print("\nNote: RC4 is simple stream cipher - same size in/out, no padding!")
        print("\nDecryption:")
        print("  earlybird.exe encrypted.bin notepad.exe MySecretPass123")
        print()
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    password = sys.argv[3]
    
    print("\n" + "=" * 60)
    print("  RC4 Payload Encryptor")
    print("=" * 60 + "\n")
    
    if not encrypt_payload(input_file, output_file, password):
        sys.exit(1)
    
    print("=" * 60)


if __name__ == "__main__":
    main()
