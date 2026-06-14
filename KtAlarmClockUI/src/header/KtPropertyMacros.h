#ifndef KtPropertyMacros_H
#define KtPropertyMacros_H

#define KT_Q_PROPERTY_AUTO(TYPE, M)                                                               \
    Q_PROPERTY(TYPE M READ M WRITE M NOTIFY M##Changed FINAL)                                     \
public:                                                                                           \
    Q_SIGNAL void M##Changed();                                                                   \
    void M(const TYPE& in_##M) {                                                                  \
        if (in_##M == m_##M)                                                                      \
            return;                                                                               \
        m_##M = in_##M;                                                                           \
        Q_EMIT M##Changed();                                                                      \
    }                                                                                             \
    TYPE M() const {                                                                              \
        return m_##M;                                                                             \
    }                                                                                             \
                                                                                                  \
private:                                                                                          \
    TYPE m_##M;

#define KT_Q_PROPERTY_READONLY_AUTO(TYPE, M)                                                      \
    Q_PROPERTY(TYPE M READ M NOTIFY M##Changed FINAL)                                             \
public:                                                                                           \
    Q_SIGNAL void M##Changed();                                                                   \
    void M(const TYPE& in_##M) {                                                                  \
        if (in_##M == m_##M)                                                                      \
            return;                                                                               \
        m_##M = in_##M;                                                                           \
        Q_EMIT M##Changed();                                                                      \
    }                                                                                             \
    TYPE M() const {                                                                              \
        return m_##M;                                                                             \
    }                                                                                             \
                                                                                                  \
private:                                                                                          \
    TYPE m_##M;

#endif // KtPropertyMacros_H
