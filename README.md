Working on an esp32 routine to store data in rtc memory. previous attempts have not been good, with finding out that it seems you can not use a templates structure within rtc memory. Complier or something seems to evaporate the intention and nothing persists through deep sleep cyles and i have not seen a message from the compiler to indicate.
So I am working on a Nomn Templates version that uses a simple buffer using multiple arrays each set with the data type required for my future data logging requirements.
Eventually the plan will be to data log 3 or 4 sets of data via bluetooth to database, and using deep sleep between transmissions to preserve battery life

(Picture attached to improve aesthetics slightly)
![IMG_0402](https://github.com/user-attachments/assets/a5b2040c-b04d-492b-a9f8-5e9e0cb34bb5)
