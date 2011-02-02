/* Copyright (c) 1998-2007 Pragmatic C Software Corp. */

/* === INSERT LICENSE === */

extern int __dig_main(int, char **);

/*
 * dummy top main module - for PLI user can supply own
 */
int main(int argc, char **argv)
{
 int rv1;

 rv1 = __dig_main(argc, argv);
 return(rv1);
}
