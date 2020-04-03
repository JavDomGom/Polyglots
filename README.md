# Polyglots

## JPEG data structure
The following image shows in detail the data structure of which a JPEG file is composed.
<p align="center"><img src="https://github.com/JavierDominguezGomez/Polyglots/blob/master/JPEG.jpg"></p>
<br>
To edit the content of the image we will use `Radare2`.

## Install Radare2
1. Run update command to update package repositories and get latest package information.
    ```bash
    ~$ sudo apt-get update -y
    ```
2. Run the install command with -y flag to quickly install the packages and dependencies.
    ```bash
    ~$ sudo apt-get install -y radare2
    ```

## Image to work
<p align="center"><img src="https://github.com/JavierDominguezGomez/Polyglots/blob/master/dog.jpg"></p>
<br>

## Add data to image
1. Use Radare to add data into the image with the following command:
    ```bash
    ~$ r2 -w dog.jpg
    ```
2. We go into visual mode by typing `V` and pressing enter.
    ```bash
    [0x00000000]> V
    ```
    The content of the image will look as follows:
    ```
    [0x00000000 0% 2968 dog.jpg]> xc
    - offset -   0 1  2 3  4 5  6 7  8 9  A B  C D  E F  0123456789ABCDEF  comment
    0x00000000  ffd8 ffe0 0010 4a46 4946 0001 0101 0048  ......JFIF.....H
    0x00000010  0048 0000 ffdb 0043 0006 0405 0605 0406  .H.....C........
    0x00000020  0605 0607 0706 080a 100a 0a09 090a 140e  ................
    0x00000030  0f0c 1017 1418 1817 1416 161a 1d25 1f1a  .............%..
    0x00000040  1b23 1c16 1620 2c20 2326 2729 2a29 191f  .#... , #&')*)..
    0x00000050  2d30 2d28 3025 2829 28ff db00 4301 0707  -0-(0%()(...C...
    0x00000060  070a 080a 130a 0a13 281a 161a 2828 2828  ........(...((((
    ...
    ```

3. From the offset `0x00000002` there are 2 bytes with value` ffe0` that correspond to an OPCODE or *marker* that is used to indicate by the following 2 bytes the number of bytes that this code fragment will occupy before encountering the next *marker*. In this case these 2 bytes have the hexadecimal value `0010`, that is, 16 in decimal, therefore the message in this section consists of the following 16 bytes: `0010 4a46 4946 0001 0101 0048 0048 0000`.

4. First modify the 2 bytes `0010` that indicate the size of the message. To do this press `C` to activate the cursor and to scroll through each byte. Put the cursor on the offset `0x00000004`. First change the byte `00` to `01`, the smallest possible non-zero positive integer value. Finally change the byte `10` to `23`, which is the hexadecimal value that represents the ASCII character of the `#` pad.
    ```
    [0x00000000 + 5> * INSERT MODE *
    - offset - | 0 1  2 3  4 5  6 7  8 9  A B  C D  E F| 0123456789ABCDEF  comment
    0x00000000 |ffd8 ffe0 0110 4a46 4946 0001 0101 0048| ......JFIF.....H
    0x00000010 |0048 0000 ffdb 0043 0006 0405 0605 0406| .H.....C........
    ```
The purpose of modifying these two bytes is, on the one hand, to extend the length of the header an additional number of bytes and, on the other hand, to indicate that the code that I will add in the following points will be a comment, since it begins with a pad on the left.

5. Note that the size of the original header was marked with a value of `0x0010` and now is `0x0123`, so I have to add bytes for the header to match in size with the new value. To calculate the number of bytes that I have to add to the header I can use the `rax2` tool, which is a help of `Radare2` and we will use it as follows. First we will calculate the bytes that `0x0010` represents:
    ```bash
    ~$ rax2 0x0010
    16
    ```
Then I calculate the bytes that the new value `0x0123` represents:
    ```bash
    ~$ rax2 0x0123
    291
    ```
Now I calculate the difference between `291` and` 16`, and the result is `275`:
    ```bash
    ~$ echo "291-16" | bc
    275
    ```

6. Now I know we have to add an additional `275` bytes to the existing header. To do this I open `Radare2` again in write mode:
    ```bash
    ~$
    ```
I move the cursor to offset `0x00000014`.
    ```
    [0x00000000 0% 2968 (0x14:-1=1)]> xc
    - offset - | 0 1  2 3  4 5  6 7  8 9  A B  C D  E F| 0123456789ABCDEF comment
    0x00000000 |ffd8 ffe0 0123 4a46 4946 0001 0101 0048| .....#JFIF.....H
    0x00000010 |0048 0000 ffdb 0043 0006 0405 0605 0406| .H.....C........
    0x00000020 |0605 0607 0706 080a 100a 0a09 090a 140e| ................
    ```
