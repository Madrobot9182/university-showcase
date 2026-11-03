SELECT DISTINCT m.name, m.email
FROM members m , waitlists w, borrowings b
WHERE w.book_id = b.book_id
AND w.member = m.email
AND m.email = b.member;
