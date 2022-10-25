#include "triangle.h"

std::optional<HitData> Triangle::hit(const Ray& r, float low, float high) const
{
    std::optional<HitData> data = std::nullopt;

    if(dot(r.direction(),m_normal)==0) {return data;} //ray and triangle are parallel

    //Write all coefficients of the matrix... (p. 78 in Shirley)
    //LHS
    const auto A{a().x() - b().x()};
    const auto B{a().y() - b().y()};
    const auto C{a().z() - b().z()};
    const auto D{a().x() - c().x()};
    const auto E{a().y() - c().y()};
    const auto F{a().z() - c().z()};
    const auto G{r.direction().x()};
    const auto H{r.direction().y()};
    const auto I{r.direction().z()};

    //RHS
    const auto J{a().x() - r.direction().x()};
    const auto K{a().y() - r.direction().y()};
    const auto L{a().z() - r.direction().z()};


    const auto M{A*(E*I-H*F) + B*(G*F-D*I) + C*(D*H-E*G)};
    assert(M!=0.f); //Avoid division by 0 errors

    const auto gamma{( I*(A*K-J*B) + H*(J*C-A*L) + G*(B*L-K*C) ) / M};
    if(gamma < 0.f || gamma > 1.f) {return data;}

    const auto beta{( J*(E*I - H*F) + K*(G*F - D*I) + L*(D*H- E*G) )/ M};
    if(beta<0.f || beta > 1.f - gamma) {return data;}

    const auto t{-1.f*(F*(A*K - J*B) + E*(J*C - A*L) + D*(B*L - K*C)) / M};

    if(t > high || t < low) {return data;} //If parameter is outside the range, ignore it
    return HitData{t,r.at(t),outward_normal(r,t), mat_ptr};

}
