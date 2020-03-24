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
3. Pressing `C` will enter courses mode, which will allow us to scroll through each byte of the image. Put the cursor in byte `4` of offset `0x00000000`, where initially you can find the byte `00`. Let's change this byte `00` to `01`, the smallest possible value. For this we need to press `i` to enter *insert* mode and write `01`. It should be as follows:
    ```
    [0x00000000 + 5> * INSERT MODE *
    - offset - | 0 1  2 3  4 5  6 7  8 9  A B  C D  E F| 0123456789ABCDEF  comment
    0x00000000 |ffd8 ffe0 0110 4a46 4946 0001 0101 0048| ......JFIF.....H
    0x00000010 |0048 0000 ffdb 0043 0006 0405 0605 0406| .H.....C........
    ```

4. Once this byte has been edited, the cursor automatically moves to byte `05` of offset `0x00000000`, where we initially found byte `10`. This byte represents the size of this section of bytes, in this case it has a value of `10` in hexadecimal, that's 16 in decimal, therefore this section has a total size of 16 bytes. We are going to change this value by byte `23`, which corresponds to the ASCII character `#` and it will work with a comment in the code.
    ```
    [0x00000000 + 6> * INSERT MODE *
    - offset - | 0 1  2 3  4 5  6 7  8 9  A B  C D  E F| 0123456789ABCDEF  comment
    0x00000000 |ffd8 ffe0 0123 4a46 4946 0001 0101 0048| .....#JFIF.....H
    0x00000010 |0048 0000 ffdb 0043 0006 0405 0605 0406| .H.....C........
    ```
