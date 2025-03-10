def is_valid_hex(hex_num):
    valid_chars = set('0123456789abcdefABCDEF')
    return set(hex_num).issubset(valid_chars)

def continueProgram():
    print("Enter 'y' to continue: ", end=" ")
    if input() == 'y':
        return True
    return False


while True:
    hex_num = input("\nHexadecimal value: ")
    if hex_num.startswith('0x'):
        hex_num = hex_num[2:]

    if is_valid_hex(hex_num):
        dec_num = int(hex_num, 16)
        print("Decimal value: ", dec_num)
    else:
        print("Input error. Retry input")

    if not continueProgram():
        break