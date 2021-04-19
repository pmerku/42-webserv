++++++++
++++++++++
++++++++++
++++++++++
++++++++++

zero ascii character at 0x0

>
+++++++++
9 at 0x1

countdown from 9 to 1
[
    [
        > + increment 0x2
        < - decrement 0x1
        < + increment 0x0 (output)
        > make loop check 0x1
    ]

    F
    0x0 is now ascii of 0x1 so lets print it
    <.>

    reset
    > set to 0x2
    [
        - decrement 0x2
        < + increment 0x1
        < - decrement 0x0
        >> make loop check 0x2
    ]
    < set to 0x1

    - decrement 0x1 to continue to next number
]
