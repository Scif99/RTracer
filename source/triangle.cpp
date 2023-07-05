#include "triangle.h"

std::optional<HitData> Triangle::Hit(const Ray& r, float low, float high) const
{
    std::optional<HitData> data = std::nullopt;

    if(Dot(r.Direction(),m_normal)==0) {return data;} //ray and triangle are parallel

    //Write all coefficients of the matrix... (p. 78 in Shirley)
    //LHS
    const auto A{V_1().X() - V_2().X()};
    const auto B{V_1().Y() - V_2().Y()};
    const auto C{V_1().Z() - V_2().Z()};
    const auto D{V_1().X() - V_3().X()};
    const auto E{V_1().Y() - V_3().Y()};
    const auto F{V_1().Z() - V_3().Z()};
    const auto G{r.Direction().X()};
    const auto H{r.Direction().Y()};
    const auto I{r.Direction().Z()};

    //RHS
    const auto J{V_1().X() - r.Direction().X()};
    const auto K{V_1().Y() - r.Direction().Y()};
    const auto L{V_1().Z() - r.Direction().Z()};


    const auto M{A*(E*I-H*F) + B*(G*F-D*I) + C*(D*H-E*G)};
    assert(M!=0.f); //Avoid division by 0 errors

    const auto gamma{( I*(A*K-J*B) + H*(J*C-A*L) + G*(B*L-K*C) ) / M};
    if(gamma < 0.f || gamma > 1.f) {return data;}

    const auto beta{( J*(E*I - H*F) + K*(G*F - D*I) + L*(D*H- E*G) )/ M};
    if(beta<0.f || beta > 1.f - gamma) {return data;}

    const auto t{-1.f*(F*(A*K - J*B) + E*(J*C - A*L) + D*(B*L - K*C)) / M};

    if(t > high || t < low) {return data;} //If parameter is outside the range, ignore it
    return HitData{ t,
                    r.At(t),
                    m_normal, 
                    mat_ptr
    };

}
