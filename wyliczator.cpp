#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct Point {
    int x;
    int y;
};

struct Segment {
    struct Point *start;
    struct Point *end;
};

int is_ccw(struct Point *a, struct Point *b, struct Point *c)
{
    return (c->y - a->y) * (b->x - a->x) > (b->y - a->y) * (c->x - a->x);
}

int intersect(struct Segment *x, struct Segment *y)
{
    struct Point *a = x->start;
    struct Point *b = x->end;
    struct Point *c = y->start;
    struct Point *d = y->end;
    return is_ccw(a, c, d) != is_ccw(b, c, d)
        && is_ccw(a, b, c) != is_ccw(a, b, d);
}

int find_any_intersection(struct Segment *suspect,
                          struct Segment *victims, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        if (intersect(suspect, &victims[i]))
            return 1;
    }
    return 0;
}

void find_segments(struct Point *red, struct Point *green,
                   struct Segment *res, int n)
{
    // our state consists of:
    //  step:             how many red points are mathed
    //  green_iter[step]: where to start with green points during $step
    //  green_taken[n]:   bitmap of matched green points
    int step = 0;
    int *green_iter, *green_taken;
    unsigned long backtracks = 0;

    green_taken = (int *)calloc(n, sizeof(int));
    green_iter  = (int *)calloc(n, sizeof(int));

    while (step < n) {
        int i;
        // try to mate the next red point with any remaining green point
        res[step].start = &red[step];
        // pick the smallest green_iter that we haven't checked before
        // (initially this is 0)

        for (i = green_iter[step]; i < n; i++) {
            if (green_taken[i]) continue;
            res[step].end = &green[i];
            if (find_any_intersection(&res[step], res, step)) {
                continue;
            }
            break;
        }
        if (i == n) {
            // backtrack
            green_taken[green_iter[step - 1]] = 0;
            green_iter[step] = 0;
            step--;
            green_iter[step]++;
            assert(step >= 0);
            backtracks++;
        } else {
            green_taken[i] = 1;
            green_iter[step] = i;
            step++;
        }
    }

    free(green_iter);
    free(green_taken);
    fprintf(stderr,"Solution found, backtracked %lu times\n",backtracks);
}

int compare_points(const void *a, const void *b)
{
    struct Point *c = (struct Point *)a;
    struct Point *d = (struct Point *)b;
    if (c->y > d->y) {
        return -1;
    } else {
        return 1;
    }
}

int main(int argc, char *argv[])
{
    char buf[64];
    struct Point   *red    = NULL;
    struct Point   *green  = NULL;
    struct Segment *result = NULL;
    int n;
    int i;

    fgets(buf, sizeof(buf), stdin);
    if (sscanf(buf, "%d\n", &n) != 1) {
        fputs("First line should be the number of pairs\n", stderr); 
        return 2;
    }

    red   = (struct Point *)malloc(sizeof(struct Point) * n);
    green = (struct Point *)malloc(sizeof(struct Point) * n);

    for (i = 0; i < n; i++) {
        int a, b, c, d;

        fgets(buf, sizeof(buf), stdin);
        if (sscanf(buf, "%d %d %d %d\n", &a, &b, &c, &d) != 4) {
            fputs("Expected 4 numbers\n", stderr); 
            free(red);
            free(green);
            return 2;
        }

        red[i].x = a;
        red[i].y = b;
        green[i].x = c;
        green[i].y = d;
    }

    result = (struct Segment *)malloc(sizeof(struct Segment) * n);

    if (argc > 1 && strcmp(argv[1], "--sort") == 0) {
        qsort(red,   n, sizeof(struct Point), compare_points);
        qsort(green, n, sizeof(struct Point), compare_points);
    }

    find_segments(red, green, result, n);

    for (i = 0; i < n; i++) {
        printf("%d %d %d %d\n", result[i].start->x, result[i].start->y,
                                result[i].end->x, result[i].end->y);
    }

    free(red);
    free(green);
    free(result);

    return 0;
}
