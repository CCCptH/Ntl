export module ntl.iterator.projected;
import ntl.iterator.concepts.iterator_concepts;
import ntl.iterator.concepts.indirectly_callable;
import ntl.functional.invoke;
import ntl.type_traits;

export namespace ne {
   template<class F, class ...Is>
       requires (ConceptIndirectlyReadable<Is> && ...) && ConceptInvocable<F, TypeIteratorReference<Is>...>
   using TypeIndirectResult = TypeInvokeResult<F, TypeIteratorDifference<Is>...>;

   template<ConceptIndirectlyReadable I, ConceptIndirectlyUnaryInvocable<I> Proj>
   struct Projected {
       using ValueType = TypeUnCVRef<TypeIndirectResult<Proj&, I>>;
       TypeIndirectResult<Proj&, I> operator*() const;
   };
}