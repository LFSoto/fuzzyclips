(deftemplate temp
    0 100 C
    ((cold (z 20 70))
    (hot (s 30 80))
    )
)
(fuzzy-union    (create-fuzzy-value temp cold)
                (create-fuzzy-value temp hot))


(plot-fuzzy-value t ".+*" nil nil
    (create-fuzzy-value temp cold)
    (create-fuzzy-value temp hot)
    (fuzzy-union (create-fuzzy-value temp cold)
                 (create-fuzzy-value temp hot))
)
