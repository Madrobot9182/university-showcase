SELECT b.bid, m.email, b.end_date
FROM members m, borrowings b, books k
WHERE m.faculty = 'CS'
AND m.email = b.member
AND b.book_id = k.book_id
AND (lower(k.author) = 'john' OR lower(k.author) = 'marry');
