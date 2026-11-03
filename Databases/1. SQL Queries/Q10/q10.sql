SELECT DISTINCT m.email
FROM members m, borrowings b, book_view v
WHERE b.member = m.email AND b.book_id = v.book_id
AND v.rating > 3.5
AND v.reqcnt > (SELECT AVG(v.reqcnt) FROM book_view v);
