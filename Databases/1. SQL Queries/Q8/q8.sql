WITH penalty_cte(member, total_pen) AS (
SELECT member, COUNT(*)
FROM penalties p, borrowings b
WHERE b.bid = p.bid
GROUP BY b.member
)

SELECT c.member, c.total_pen, ifnull(COUNT(p.pid), 0) as paid_pen, ifnull(SUM(p.paid_amount), 0) as paid_amount
FROM borrowings b, penalty_cte c
LEFT JOIN penalties p ON b.bid = p.bid AND p.amount <= p.paid_amount
WHERE c.member = b.member
GROUP BY b.member;
