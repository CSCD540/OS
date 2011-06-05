milk, note, flag[3];

process p0 {

   if ((note == 9) AND (flag[1] > milk)) {
      r1 = 999;
      note = 998;
      halt;
   }
   r0 = open("file", 0);
}

process p1 {

   if ((note == 9) AND (flag[1] > milk)) {
      r1 = 999;
      note = 998;
      halt;
   }
   r0 = open("file", 0);
}