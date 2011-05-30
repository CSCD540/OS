milk, note, flag[3], milk2, note2, flag2[3];

process p0 {

   if ((note == 9) AND (flag[1] > milk)) {
      r1 = 999;
      note = 998;
      halt;
   }
   r0 = open("file", 0);
}

process p1 {

   if ((note2 == 9) AND (flag2[1] > milk2)) {
      r1 = 999;
      note2 = 998;
      halt;
   }
   r0 = open("file", 0);
}