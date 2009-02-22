// RUN: clang -fsyntax-only -verify %s
// PR3459
struct bar {
	char n[1];
};

struct foo {
	char name[(int)&((struct bar *)0)->n];
	char name2[(int)&((struct bar *)0)->n - 1]; //expected-error{{array size is negative}}
};

// PR3430
struct s {
        struct st {
                int v;
        } *ts;
};

struct st;

int foo() {
        struct st *f;
        return f->v + f[0].v;
}

// PR3642
struct pppoe_tag {
 short tag_type;
 char tag_data[];
};
struct datatag {
 struct pppoe_tag hdr; //expected-error{{variable sized type 'hdr' must be at end of struct}}
 char data;
};

