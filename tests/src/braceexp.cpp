#include <stdlib.h>
#include <string.h>


static int _rw_is_digit (int ch)
{
    return !(ch < '0' || '9' < ch);
}

static int _rw_is_lower (int ch)
{
    return !(ch < 'a' || 'z' < ch);
}

static int _rw_is_upper (int ch)
{
    return !(ch < 'A' || 'Z' < ch);
}

// search `beg' to `end' for the next unescaped open brace . if `end'
// is 0 then the search will terminate at the end of string. returns 0
// on failure.
static const char* _rw_find_open_brace (const char* beg,
                                        const char* end)
{
    bool is_escaped = false;

    for (/**/; *beg; ++beg)
    {
        if (end && !(beg < end))
            break;

        const bool is_match = (*beg == '{');
        if (!is_escaped && is_match) {
            return beg;
        }

        is_escaped = !is_escaped && (*beg == '\\');
    }

    return 0;
}

// search `beg' to `end' for the next close brace at the current brace
// depth. if `end' is 0, the search will continue until a match or end
// of string. returns 0 on failure.
static const char* _rw_find_close_brace (const char* beg,
                                         const char* end)
{
    bool is_escaped = false;

    // nested brace depth
    for (int depth = 1; *beg; ++beg)
    {
        if (end && !(beg < end))
            break;

        const bool is_open_brace = (*beg == '{');
        if (!is_escaped && is_open_brace) {
            ++depth;
        }

        const bool is_close_brace = (*beg == '}');
        if (!is_escaped && is_close_brace) {
            --depth;
        }

        is_escaped = !is_escaped && (*beg == '\\');

        if (depth == 0)
            return beg;
    }

    return 0;
}

// search `beg' to `end' for the next unescaped comma at the current
// brace depth. if `end' is 0, the search will continue until a match
// or end of string. returns 0 on failure.
static const char* _rw_find_next_comma (const char* beg,
                                        const char* end)
{
    bool is_escaped = false;

    // nested brace depth
    for (int depth = 0; *beg; ++beg)
    {
        if (end && !(beg < end))
            break;

        const bool is_open_brace = (*beg == '{');
        if (!is_escaped && is_open_brace) {
            ++depth;
        }

        const bool is_close_brace = (*beg == '}');
        if (!is_escaped && is_close_brace) {
            --depth;
        }

        const bool is_comma = (*beg == ',');
        if (!is_escaped && is_comma) {
            if (depth == 0)
                return beg;
        }

        is_escaped = !is_escaped && (*beg == '\\');
    }

    return 0;
}



// this is where most of the work is done.
struct _rw_brace_graph
{
    //
    _rw_brace_graph ();

    //
    ~_rw_brace_graph ();

    // expand `brace_expr' into `len' bytes of `buf'. if it won't fit, we
    // allocate a buffer with malloc() and return that. so the user needs
    // to free() the return buffer if it is not equal to `buf'.
    char* _C_build_and_expand (const char* brace_expr,
                               char* buf, int len, int sep);

private:

    // node for a directed-acyclic-graph that we build from the original
    // brace expression
    struct _rw_brace_node
    {
        const char* str_;
        int len_;

        _rw_brace_node* sibling_;
        _rw_brace_node* child_;
    };

    // retrieve a new node. nodes are allocated in large blocks. those
    // blocks are deallocated when this graph instance is destroyed.
    _rw_brace_node* _C_get_new_node ();

    // this function generates a dag from an arbitrary brace expression.
    // the format is `prefix{body}suffix'. prefix, and suffix can both
    // be the empty string. body may be a comma seperated list of tokens,
    // a range of tokens, or arbitrary literal text. escapes on commas
    // and braces are ignored, and left intact otherwise.
    _rw_brace_node* _C_build_anything (const char* beg, const char* end);

    // generates a dag from an arbitrary range brace expression. the
    // format is `{?..?}suffix' where both ? are alphanumeric digits
    // of the same character class.
    _rw_brace_node* _C_build_range    (const char* beg, const char* end);

    // generates a dag from an arbitrary list brace expression. the
    // format is `{a,b[,c]}suffix', where `a', `b' and `c' are full
    // brace expansions that would be processed by _C_build_anything.
    _rw_brace_node* _C_build_list     (const char* beg, const char* end);

    // generates a dag from literal text. no brace expansion is done
    // on the literal text.
    _rw_brace_node* _C_build_literal  (const char* beg, const char* end);

    // the number of nodes held by each brace buffer [see below]
    enum { _C_size = 64 };

    // buffer to avoid many small allocations
    struct _rw_brace_buffer
    {
        _rw_brace_node nodes_ [_C_size];
        _rw_brace_buffer* next_;
    };

    // the initial set of nodes is preallocated as part of this graph
    // object instance. additional blocks of nodes will be allocated
    // as is necessary by the _C_get_new_node() member.
    _rw_brace_buffer node_cache_;
    
    int used_; // number of nodes used in the last buffer
    _rw_brace_buffer* nodes_; // pointer to last allocated node buffer

    // code for generating the string

    bool _C_append (const char* beg, int n);

    char* user_buf_;   // this is the buffer
    char* string_buf_;
    int string_cap_;
    int string_len_;

    // we use this to walk the stack. we need to walk from the root
    // of the tree to each leaf. when we reach a leaf, we need a way
    // to see the path that we took to get where we are. we use this
    // path to write out each part of the full expansion.
    struct _rw_recursion_context
    {
        _rw_recursion_context (_rw_brace_node* node)
            : parent_ (0), node_ (node)
        {
        }

        _rw_recursion_context (_rw_recursion_context* parent)
            : parent_ (parent), node_ (parent->node_->child_)
        {
        }

        _rw_recursion_context* parent_;
        _rw_brace_node* node_;
    };

    // this function walks the dag, leaving a trail of context
    // objects so we can walk back to the root and write the data
    // at each level in the graph.
    bool _C_brace_expand (_rw_recursion_context* self, char sep);

    // this function writes the data at each level of the dag
    // to our internal buffer.
    bool _C_brace_expand_write (_rw_recursion_context* context);
};

_rw_brace_graph::_rw_brace_graph ()
    : used_ (0)
    , nodes_ (&node_cache_)
    , user_buf_ (0)
    , string_buf_ (0)
    , string_cap_ (0)
    , string_len_ (0)
{
    node_cache_.next_ = 0;
}

_rw_brace_graph::~_rw_brace_graph ()
{
    _rw_brace_buffer* nodes = node_cache_.next_;
    while (nodes)
    {
        _rw_brace_buffer* next = nodes->next_;
        nodes->next_ = 0;

        // deallocate this buffer
        free (nodes);

        // setup to deallocate next one
        nodes = next;
    }
}



char*
_rw_brace_graph::_C_build_and_expand (const char* brace_expr,
                                      char* buf, int len, int sep)
{
    const int brace_expr_len = strlen (brace_expr);

    _rw_brace_node* root = _C_build_anything (brace_expr,
                                              brace_expr + brace_expr_len);
    if (!root)
        return 0;

    // setup for the expansion
    user_buf_   = buf;
    string_buf_ = buf;
    string_cap_ = len;
    string_len_ = 0;

   // this helps us do the building of the output string
    _rw_recursion_context context (root);

    if (!_C_brace_expand (&context, sep))
    {
        if (string_buf_ != user_buf_)
            free (string_buf_);
        string_buf_ = 0;
    }

    // kill the last seperator with a null terminator
    else if (string_buf_)
    {
        const int pos = string_len_ < 1 ? 0 : string_len_ - 1;
        string_buf_ [pos] = '\0';
    }

    return string_buf_;
}

_rw_brace_graph::_rw_brace_node*
_rw_brace_graph::_C_get_new_node ()
{
    used_ += 1;

    // if we run out of space, grab a new block
    if (! (used_ < _C_size))
    {
        nodes_->next_
           = (_rw_brace_buffer*)malloc (sizeof (_rw_brace_buffer));
        if (!nodes_->next_)
            return 0;

        nodes_ = nodes_->next_;
        nodes_->next_ = 0;

        used_ -= _C_size;
    }

    _rw_brace_node* node = &nodes_->nodes_ [used_ - 1];
    node->str_     = 0;
    node->len_     = 0;
    node->sibling_ = 0;
    node->child_   = 0;

    return node;
}

_rw_brace_graph::_rw_brace_node*
_rw_brace_graph::_C_build_anything (const char* beg, const char* end)
{
    // 
    const char* open_brace = _rw_find_open_brace (beg, end);
    if (open_brace)
    {
        // build a node for the prefix if there is one
        _rw_brace_node* prefix = _C_get_new_node ();
        prefix->str_ = beg;
        prefix->len_ = (open_brace - beg);

        // try to build a range expansion
        _rw_brace_node* child = 0;
        
        child = _C_build_range (open_brace, end);
        if (!child) {

            // try to do a list expansion
            child = _C_build_list (open_brace, end);
            if (!child) {

                // try to do a literal expansion
                child = _C_build_literal (open_brace, end);
                if (!child) {
                    return 0;
                }
            }
        }

        // we must have a valid child pointer at this point
        prefix->child_ = child;
   
        return prefix;
    }

    // there was no open brace, so the entire text from beg to end
    // is a literal
    _rw_brace_node* prefix = _C_get_new_node ();
    if (!prefix)
        return 0;

    prefix->str_ = beg;
    prefix->len_ = (end - beg);

    return prefix;
}

_rw_brace_graph::_rw_brace_node*
_rw_brace_graph::_C_build_literal (const char* beg, const char* end)
{
    _rw_brace_node* prefix = _C_get_new_node ();
    if (!prefix)
        return 0;

    prefix->str_ = beg;
    prefix->len_ = (end - beg);

    return prefix;
}

_rw_brace_graph::_rw_brace_node*
_rw_brace_graph::_C_build_range (const char* beg, const char* end)
{
    // check for {a..z} type range expression. make sure not to
    // reference past the end of the string.
    if (   beg [0] != '{'
        || beg [1] == '\0'
        || beg [2] != '.'
        || beg [3] != '.'
        || beg [4] == '\0'
        || beg [5] != '}')
        return 0;

    // grab characters that represent the start and end of the range
    char cbeg = beg [1];
    char cend = beg [4];

    // only works if range characters are both digit, lower or upper.
    const int both_are_digit = _rw_is_digit (cbeg) && _rw_is_digit (cend);
    const int both_are_lower = _rw_is_lower (cbeg) && _rw_is_lower (cend);
    const int both_are_upper = _rw_is_upper (cbeg) && _rw_is_upper (cend);

    if (! (both_are_digit || both_are_lower || both_are_upper))
        return false;

    // these must live for duration of program
    static const char* table [] =
    {
        "0123456789",                 // 0
        "abcdefghijklmnopqrstuvwxyz", // 1
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ", // 2
    };

    const int idx = (both_are_lower << 0) +
                    (both_are_upper << 1);

    const int dir = (cbeg < cend) ? 1 : -1;

    // build the suffix
    _rw_brace_node* suffix = _C_build_anything (beg + 6, end);
    if (!suffix)
        return false; // failed to parse suffix

    _rw_brace_node* first_child = 0;
    _rw_brace_node* final_child = 0;

    // build a list of children, associate each with suffix
    for (/**/; cbeg != cend; cbeg += dir) {

        // create a child from whatever is between beg and end. that childs
        // next pointer will be the suffix we created above.
        _rw_brace_node* child = _C_get_new_node ();
        if (!child) {
            return false;
        }

        // this finds beg in our array, we could have used strchr
        child->str_ = table [idx] + (cbeg - table [idx][0]);
        child->len_ = 1;

        // track children we have created
        if (!first_child)
            first_child = child;

        if (final_child)
            final_child->sibling_ = child;
        final_child = child;

        // suffix is the suffix of the child expression
        while (child->child_)
            child = child->child_;
        child->child_ = suffix;
    }

    // create the last node in the range.
    _rw_brace_node* child = _C_get_new_node ();
    if (!child) {
        return false;
    }

    child->str_ = table [idx] + (cbeg - table [idx][0]);
    child->len_ = 1;

    // trach child
    if (!first_child)
        first_child = child;

    if (final_child)
        final_child->sibling_ = child;
    final_child = child;

    // suffix is the suffix of the child expression
    while (child->child_)
        child = child->child_;
    child->child_ = suffix;

    return first_child;
}

_rw_brace_graph::_rw_brace_node*
_rw_brace_graph::_C_build_list (const char* beg, const char* end)
{
    //
    // {abc,d{1..3}e}f
    // ^              ^
    //
    // prefix ''
    // suffix 'f'
    // middle '{abc,d{1..3}e}'

    // generate node for prefix [which is a literal, and is handled by the caller
    // generate node for the suffix [which is anything]

    // generate list of nodes for the middle [each of which are anything]
    // set the next pointer of each to the suffix, and set next pointer
    // of the prefix to the first list item.

    // d{1..3}e{1..3}
    //
    // root -> d -> 1 -> e -> 1 -> EMPTY
    //              2 //      2 //
    //              3 /       3 /

    // {1..3}{4..6}  14 15 16 24 25 26 34 35 36
    //
    // root -> 1 -> 4
    //         2 // 5
    //         3 /  6

    // we really expect that the first token is an open paren the
    // caller should have consumed the prefix before calling this
    if (*beg++ != '{')
        return false;

    // now fill in the middle, each child we create directly will
    // have a child pointer to the suffix node

    // all of the nodes in the list are children of `parent'. the
    // graph from nodes after the list are 
    const char* mid = _rw_find_next_comma (beg, end);
    if (!mid)
        return false; // no comma, then no list

    // find the end of th list expression
    const char* eol = _rw_find_close_brace (mid, end);
    if (!eol)
        return false; // no list terminator

    // build a node for the suffix.
    _rw_brace_node* suffix = _C_build_anything (eol+1, end);
    if (!suffix)
        return false; // failed to parse end

    _rw_brace_node* first_child = 0;
    _rw_brace_node* final_child = 0;

    while (mid) {
    
        // create a child from whatever is between beg and end. that childs
        // next pointer will be the suffix we created above.
        _rw_brace_node* child = _C_build_anything (beg, mid);
        if (!child) {
            return false;
        }

        if (!first_child)
            first_child = child;
        
        // append new child to end of chain
        if (final_child)
            final_child->sibling_ = child;
        final_child = child;

        // the nex pointer for this child is the suffix

        // suffix is the suffix of the child expression
        while (child->child_)
            child = child->child_;
        child->child_ = suffix;

        beg = mid + 1;
        mid = _rw_find_next_comma (beg, eol);
    }

    // okay, we have a pointer to the last comma in the list. create an
    // item for the data between the comma and the list terminator

    // '{abc,d{1..3}e}a'
    //      ^        ^ ^
    //    beg      eol end

    // build nodes from the last entry in the list
    _rw_brace_node* child = _C_build_anything (beg, eol);
    if (!child) {
        return false;
    }

    // append last child to end of chain
    final_child->sibling_ = child;
    final_child = child;

    // suffix is the suffix of the child expression
    while (child->child_)
        child = child->child_;
    child->child_ = suffix;

    // success, return first child in this expansion
    return first_child;
}

bool _rw_brace_graph::_C_append (const char* s, int n)
{
    const int new_len = string_len_ + n;

    // not enough space, grow buf
    if (! (new_len < string_cap_)) {

        // buf grows in 256 byte blocks
        int new_cap = string_cap_;
        while (! (new_len < new_cap))
            new_cap += 256;

        char* new_buf = (char*)malloc (new_cap);
        if (!new_buf)
            return false;

        // copy the old data
        memcpy (new_buf, string_buf_, string_len_);

        // copy the new data
        memcpy (new_buf + string_len_, s, n);
        new_buf [new_len] = '\0';

        // don't free until after append because `s' may
        // be in string_buf_
        if (string_buf_ != user_buf_)
            free (string_buf_);

        string_cap_ = new_cap;
        string_len_ = new_len;
        string_buf_ = new_buf;
    }

    // just copy the data
    else {
        memcpy (string_buf_ + string_len_, s, n);
        string_buf_ [new_len] = '\0';

        string_len_ = new_len;
    }

    return true;
}


bool _rw_brace_graph::_C_brace_expand_write (_rw_recursion_context* context)
{
    if (context->parent_) {
        if (!_C_brace_expand_write (context->parent_))
            return false;
    }

    // write the data at this level
    //return _C_append (context->node_->str_, context->node_->len_);

    bool is_escaped = false;

    const char* beg = context->node_->str_;
    for (int n = 0; n < context->node_->len_; ++n, ++beg) {

        is_escaped = !is_escaped && (*beg == '\\');
        if (!is_escaped) {
            if (!_C_append (beg, 1))
                return false;
        }
    }

    return true;
}

bool _rw_brace_graph::_C_brace_expand (_rw_recursion_context* self, char sep)
{
    // if this node has no children or siblings, we have found a full
    // expansion.
    if (!self->node_ ||
        !self->node_->sibling_ && !self->node_->child_) {

        const int length_before = string_len_;

        // use recursion again to walk back to the root the graph and
        // write each contexts data as we unwind back toward the leaf
        if (!_C_brace_expand_write (self))
            return false;

        const int length_after = string_len_;

        // don't write a seperator if we wrote no data
        if (length_before != length_after && !_C_append (&sep, 1))
            return false;

        return true;
    }
    
    // iterate through all of the children of the node, thus finding all
    // other combinations
    _rw_recursion_context context (self);
    while (context.node_) {

        if (!_C_brace_expand (&context, sep))
            return false;

        context.node_ = context.node_->sibling_;
    }

    return true;
}

//
// returns 0 if brace_expr is not a valid brace expression, else a
// pointer to the buf that stores the expanded expression. if the
// returned value is not the same pointer as the input parameter
// expanded, then the user must deallocate the pointer with a call
// to free().
char* rw_brace_expand (const char* brace_expr, char* s, int n, int sep)
{
    if (!brace_expr)
        return 0;

    _rw_brace_graph graph;

    // build the graph, and then expand it into buf
    char* buf = graph._C_build_and_expand (brace_expr, s, n, sep);
    if (!buf)
        return 0;

    return buf;
}

