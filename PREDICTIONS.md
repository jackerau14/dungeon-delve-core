# Concept Lab 2 — Prediction Table

**Name:**  Jack Rau    **Date:** 10/31/2026


---

## Step 1 — Predict.

The predictions below are from the earlier draft in our conversation. Check that they match what you actually predicted before running the program.

| # | The call | Copies | Moves | What the function receives |
|:--|:--|:--:|:--:|:--|
| 1 | `by_value(original)` | 1 | 0 | A copy of `original` called `hero-copy` |
| 2 | `by_const_ref(original)` | 0 | 0 | A read-only reference to `hero` |
| 3 | `by_ref(original)` | 0 | 0 | A reference to `hero`, which the function changes to `hero!` |
| 4 | `by_pointer(&original)` | 0 | 0 | A pointer to `hero` |
| 5 | `by_value(make_probe("temp"))` | 0 | 0 | A temporary `Probe` named `temp` |
| 6 | `Holder h{original}` | 1 | 0 | A new `Holder` containing a copy of `hero` |

**Predicted totals:** copies **2**; moves **0**

---

## Step 2 — Run it.

The following actual counts were verified by compiling and running `prediction_table.cpp` with a C++17 compiler.

| # | Actual copies | Actual moves | Matched my prediction? |
|:--|:--:|:--:|:--:|
| 1 | 1 | 0 | Yes |
| 2 | 0 | 0 | Yes |
| 3 | 0 | 0 | Yes |
| 4 | 0 | 0 | Yes |
| 5 | 0 | 0 | Yes |
| 6 | 1 | 1 | No |

**Actual totals:** copies **2**; moves **1**

---

## Step 3 — Reconcile.

### Row 6

**What I expected:** I expected `Holder h{original}` to make one copy and no moves because I was passing in an existing object.

**What actually happened:** The program made one copy when `original` was passed to the constructor's value parameter, and then it made one move when that parameter was moved into the `held` member.

---

## Step 4 — The closing question

**Which one of the six calls would you change, and to what?**

**My answer:** I would change call 1 so the function takes `const Probe&` instead of `Probe` by value. Call 1 makes a full copy of the `Probe`, including its string, even though the function only prints the name and does not need to change it. Passing by const reference would avoid that extra copy and the cost of creating and destroying it.

---

## What is being graded

| Requirement | Status |
|:--|:--|
| All six predictions recorded before the first compile | Confirm the earlier draft matches your original predictions |
| Reconciliation for every incorrect row | Included for row 6 based on the draft predictions |
| Closing recommendation names a cost | Included |
| Reconciliation write-up | Included |
