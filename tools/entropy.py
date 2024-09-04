#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
Calculate the Shannon entropy of an ELF file.

A rewrite of the original script by Will Summerhill for ELF files.
https://gist.github.com/wsummerhill/a5a2068e717b5c290ab345c05ef99fcc
"""
import argparse
import math
import os
import sys

from elftools.elf.elffile import ELFFile


def main():
    """
    Main function to parse arguments and calculate Shannon entropy.

    :param args: Command line arguments
    """
    parser = argparse.ArgumentParser(
        description="Calculate Shannon entropy of input file"
    )
    parser.add_argument("-f", "--file", help="Input file", required=True)
    parser.add_argument(
        "-e",
        "--elf",
        help="Specify if input file is ELF for additional analysis",
        action="store_true",
    )
    args = parser.parse_args()
    file_path = args.file
    is_elf = args.elf

    if not os.path.exists(file_path):
        print(f"[-] ERROR! File not found: {file_path}")
        sys.exit(1)

    # Calculate entropy of input file
    entropy = calculate_entropy_of_file(file_path)
    print(f"File Shannon Entropy: {entropy}")

    # Further analysis on sections if input file is ELF
    if is_elf:
        analyze_elf_file(file_path)


def shannon_entropy(data):
    """
    Calculate the Shannon entropy of the given data.

    :param data: Bytes data to calculate entropy
    :return: Shannon entropy value
    """
    byte_counts = [0] * 256
    for byte in data:
        byte_counts[byte] += 1

    total_bytes = len(data)
    probabilities = [count / total_bytes for count in byte_counts if count > 0]

    entropy = -sum(p * math.log2(p) for p in probabilities)
    return entropy


def calculate_entropy_of_file(file_path):
    """
    Calculate the entropy of a file.

    :param file_path: Path to the file
    :return: Shannon entropy of the file
    """
    with open(file_path, "rb") as file:
        data = file.read()
        return shannon_entropy(data)


def analyze_elf_file(file_path):
    """
    Analyze the ELF file and print entropy for each section.

    :param file_path: Path to the ELF file
    """
    with open(file_path, "rb") as file:
        elf_file = ELFFile(file)
        for section in elf_file.iter_sections():
            section_name = section.name
            section_data = section.data()
            entropy = shannon_entropy(section_data)
            print(f"Section: {section_name}")
            print(f"Section Entropy: {entropy}")


if __name__ == "__main__":
    main()
