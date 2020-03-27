# Polyglots

## JPEG data structure
The following image shows in detail the data structure of which a JPEG file is composed.
<p align="center"><img src="https://github.com/JavierDominguezGomez/Polyglots/blob/master/JPEG.jpg"></p>
<br>
To edit the content of the image we will use Radare2.

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

4. First I am going to modify the 2 bytes `0010` that indicate the size of the message. To do this press `C` to activate the cursor and to scroll through each byte. I put the cursor on the offset `0x00000004`. I first change the byte `00` to `01`, the smallest possible non-zero positive integer value. Finally I change the byte `10` to `23`, which is the hexadecimal value that represents the ASCII character of the `#` pad.
    ```
    [0x00000000 + 5> * INSERT MODE *
    - offset - | 0 1  2 3  4 5  6 7  8 9  A B  C D  E F| 0123456789ABCDEF  comment
    0x00000000 |ffd8 ffe0 0110 4a46 4946 0001 0101 0048| ......JFIF.....H
    0x00000010 |0048 0000 ffdb 0043 0006 0405 0605 0406| .H.....C........
    ```

5. Once this byte has been edited, the cursor automatically moves to byte `05` of offset `0x00000000`, where we initially found byte `10`. This byte represents the size of this section of bytes, in this case it has a value of `10` in hexadecimal, that's 16 in decimal, therefore this section has a total size of 16 bytes. We are going to change this value by byte `23`, which corresponds to the ASCII character `#` and it will work with a comment in the code.
    ```
    [0x00000000 + 6> * INSERT MODE *
    - offset - | 0 1  2 3  4 5  6 7  8 9  A B  C D  E F| 0123456789ABCDEF  comment
    0x00000000 |ffd8 ffe0 0123 4a46 4946 0001 0101 0048| .....#JFIF.....H
    0x00000010 |0048 0000 ffdb 0043 0006 0405 0605 0406| .H.....C........
    ```
The purpose of modifying these two bytes is to add as a comment in the code a series of bytes that I am going to insert in the following points.
